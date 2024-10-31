#include "mainwindow.h"
#include "./ui_mainwindow.h"

const QString configPath = CLIENT_CONFIG_FILE_PATH;

std::string MainWindow::getRandomString() {
    std::srand((unsigned int)time(NULL));
    int len = 1 + rand() % 10;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    socket = NULL;
    ui->setupUi(this);
    ui->numberEdit->setValidator(new QIntValidator);
    ui->responseEdit->setReadOnly(true);
    // DEFAULT
    hostname = "localhost";
    port = 5672;
    exchange = "amq.direct";
    routingkey = "test";
    request.set_id(getRandomString());
    Logger::log(DEBUG, "Genered ID for protobuf is " + QString::fromStdString(request.id()));

    const QFileInfo configFileInfo(configPath);
    if (!configFileInfo.exists() or configFileInfo.size() == 0) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to client were set");
    }
    else {
        const QSettings settings(configPath, QSettings::IniFormat);
        hostname = settings.value("Rabbit/Host").toString();
        port = settings.value("Rabbit/Port").toInt();
        exchange = settings.value("Rabbit/Exchange").toString();
        routingkey = settings.value("Rabbit/Routingkey").toString();
        Logger::log(INFO, "Got hostname, port and routingkey from config to client");
    }

    Logger::log(DEBUG, "hostname=" + hostname + " port=" + QString::number(port) + " exchange=" + exchange + " routingkey=" + routingkey);

    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        die("creating TCP socket");
    }
    status = amqp_socket_open(socket, hostname.toStdString().c_str(), port);
    if (status) {
        die("opening TCP socket");
    }
    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                 "guest", "guest"),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

    Logger::log(DEBUG, "New connection for RabbitMQ is established");

    startListening();

}


MainWindow::~MainWindow() {
    if (listenThread) {
        listenThread->requestInterruption();
        listenThread->wait();
        delete listenThread;
        Logger::log(DEBUG, "ListenThread is interrupted");
    }
    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                      "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");
    Logger::log(DEBUG, "Connection for RabbitMQ is closed");
    delete ui;
    Logger::log(DEBUG, "MainWindow is closed");
}

void MainWindow::startListening() {
    listenThread = new QueueListener(conn, exchange, QString::fromStdString(request.id()));
    connect(listenThread, &QueueListener::messageReceived, this, &MainWindow::onMessageReceived);
    listenThread->start();
}

void MainWindow::onMessageReceived(QString num) {
    ui->responseEdit->setText(num);
}


void MainWindow::on_sendButton_clicked() {
    Logger::log(DEBUG, "Send settings button clicked");
    if (ui->numberEdit->text() == "") {
        QMessageBox::warning(this, "Warning", "Please write number!");
        Logger::log(WARNING, "Please write number!");
        return;
    }
    ui->responseEdit->setText("");
    request.set_req(ui->numberEdit->text().toInt());
    std::string m_str;
    request.SerializeToString(&m_str);
    Logger::log(DEBUG, "Serialized proto-message: " + QString::fromStdString(m_str));

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("application/x-protobuf");
    props.delivery_mode = 2;
    die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange.toStdString().c_str()),
                                    amqp_cstring_bytes(routingkey.toStdString().c_str()), 0, 0,
                                    &props, amqp_cstring_bytes(m_str.c_str())),
                 "Publishing");
    Logger::log(INFO, "Proto-message sent to server");

    ui->numberEdit->setText("");
}


void MainWindow::on_editSettingsButton_clicked() {
    Logger::log(DEBUG, "Edit settings button clicked");
    dialog = new DialogToEditSettings(this);

    // NEW SETTINGS GOT
    connect(dialog, &DialogToEditSettings::settingsChanged, this, [this](QSettings *settings) {
        Logger::log(INFO, "New settings recieved");
        reloadSettings(settings);
    });

    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowFullscreenButtonHint);
    dialog->exec();
}

void MainWindow::reloadSettings(QSettings *settings) {
    Logger::log(INFO, "Reloading settings and connection to RabbitMQ");

    if (listenThread) {
        listenThread->requestInterruption();
        listenThread->wait();
        delete listenThread;
        listenThread = nullptr;
        Logger::log(DEBUG, "ListenThread is interrupted");
    }

    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                      "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");
    Logger::log(DEBUG, "Connection for RabbitMQ is closed");


    hostname = settings->value("Rabbit/Host").toString();
    port = settings->value("Rabbit/Port").toInt();
    routingkey = settings->value("Rabbit/Routingkey").toString();
    Logger::log(DEBUG, "Settings updated");

    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        die("creating TCP socket");
    }
    status = amqp_socket_open(socket, hostname.toStdString().c_str(), port);
    if (status) {
        die("opening TCP socket");
    }
    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                 "guest", "guest"),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

    Logger::log(DEBUG, "New connection for RabbitMQ is established");

    startListening();
}

