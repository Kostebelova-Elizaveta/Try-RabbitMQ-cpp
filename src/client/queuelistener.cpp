#include "queuelistener.h"

QueueListener::QueueListener(amqp_connection_state_t &conn, QString &exchange, QString id)
    : m_conn(conn), m_exchange(exchange), m_id(id)
{
}

void QueueListener::run() {
    struct timeval tval;
    struct timeval *tv;
    tv = &tval;
    tv->tv_sec = 1;
    tv->tv_usec = 0;

    Logger::log(DEBUG, "QueueListener is launched");

    amqp_queue_declare_ok_t *r = amqp_queue_declare(
                m_conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Declaring queue");
    amqp_bytes_t queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL) {
        Logger::log(ERROR, "Out of memory while copying queue name");
        return;
    }

    amqp_queue_bind(m_conn, 1, queuename, amqp_cstring_bytes(m_exchange.toStdString().c_str()),
                    amqp_cstring_bytes(m_id.toStdString().c_str()), amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Binding queue");
    amqp_basic_consume(m_conn, 1, queuename, amqp_empty_bytes, 0, 1, 0,
                       amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Consuming");

    Logger::log(DEBUG, "Queue is binded, start listening");

    while (!isInterruptionRequested()) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;
        amqp_maybe_release_buffers(m_conn);
        res = amqp_consume_message(m_conn, &envelope, tv, 0);
        if (AMQP_RESPONSE_NORMAL == res.reply_type) {
            TestTask::Messages::Response response;
            response.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);
            Logger::log(DEBUG, "Recieved response from server: " + QString::number(response.res()));
            emit messageReceived(QString::number(response.res()));
            amqp_destroy_envelope(&envelope);
        }
    }

    amqp_bytes_free(queuename);
}

