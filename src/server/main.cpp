#include <QCoreApplication>
#include "logger.h"
#include "Messages.pb.h"
#include "rabbitserver.h"

const QString configPath = SERVER_CONFIG_FILE_PATH;


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Logger::initFromConfig(configPath);
    RabbitServer *server = new RabbitServer();
    server->initFromConfig(configPath);

    server->start();

    delete server;

    return a.exec();
}
