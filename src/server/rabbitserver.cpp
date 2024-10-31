#include "rabbitserver.h"

RabbitServer::RabbitServer()
{
    //DEFAULT
    hostname = "localhost";
    port = 5672;
    exchange = "amq.direct";
    bindingkey = "test";
}


void RabbitServer::initFromConfig(const QString &configFilePath) {
    const QFileInfo configFileInfo(configFilePath);
    if (!configFileInfo.exists() or configFileInfo.size() == 0) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to server were set");
    }
    else {
        const QSettings settings(configFilePath, QSettings::IniFormat);
        hostname = settings.value("Rabbit/Host").toString();
        port = settings.value("Rabbit/Port").toInt();
        exchange = settings.value("Rabbit/Exchange").toString();
        bindingkey = settings.value("Rabbit/Bindingkey").toString();
        Logger::log(INFO, "Got hostname, port, exchange and bindingkey from config to server");
    }
    Logger::log(DEBUG, "hostname=" + hostname + " port=" + QString::number(port) + " exchange=" + exchange + " bindingkey=" + bindingkey);
}


TestTask::Messages::Response RabbitServer::doubleReq(TestTask::Messages::Request request) {
    if (request.id() == "") {
        throw std::runtime_error("Invalid request");
    }
    TestTask::Messages::Response response;
    response.set_id(request.id());
    response.set_res(request.req()*2);
    return response;
}


void RabbitServer::start() {
    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        die("creating TCP socket");
    }
    const int status = amqp_socket_open(socket, hostname.toStdString().c_str(), port);
    if (status) {
        die("opening TCP socket");
    }
    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                 "guest", "guest"),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");
    Logger::log(INFO, "Connection for RabbitMQ is established");

    amqp_queue_declare_ok_t *r = amqp_queue_declare(
                conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Declaring queue");
    amqp_bytes_t queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL) {
        Logger::log(ERROR, "Out of memory while copying queue name");
        return;
    }

    amqp_queue_bind(conn, 1, queuename, amqp_cstring_bytes(exchange.toStdString().c_str()),
                    amqp_cstring_bytes(bindingkey.toStdString().c_str()), amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Binding queue");
    amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0,
                       amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    Logger::log(DEBUG, "Queue is binded, start listening");

    for (;;) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;
        amqp_maybe_release_buffers(conn);
        res = amqp_consume_message(conn, &envelope, NULL, 0);
        if (AMQP_RESPONSE_NORMAL == res.reply_type) {
            Logger::log(INFO, "Got request");
            TestTask::Messages::Request request;
            request.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);

            Logger::log(DEBUG, "Request is parsed from array, parameters are: "
                               "id=" + QString::fromStdString(request.id())
                        + " and req=" + QString::number(request.req()));
            TestTask::Messages::Response response = doubleReq(request);
            std::string s_response;
            response.SerializeToString(&s_response);
            Logger::log(DEBUG, "req is doubled and response is serialized");
            amqp_destroy_envelope(&envelope);



            amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
            props.content_type = amqp_cstring_bytes("application/x-protobuf");
            props.delivery_mode = 2;
            die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange.toStdString().c_str()),
                                            amqp_cstring_bytes(response.id().c_str()), 0, 0,
                                            &props, amqp_cstring_bytes(s_response.c_str())),
                         "Publishing");

            Logger::log(INFO, "Response is published");
        }

    }

    amqp_bytes_free(queuename);
    Logger::log(DEBUG, "Free queuename");

    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                      "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");

    Logger::log(INFO, "Connection for RabbitMQ is closed");
}

QString RabbitServer::getHostname() {
    return hostname;
}


int RabbitServer::getPort() {
    return port;
}


QString RabbitServer::getExchange() {
    return exchange;
}


QString RabbitServer::getBingkey() {
    return bindingkey;
}
