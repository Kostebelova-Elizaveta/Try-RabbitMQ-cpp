#include "mainwindow.h"

#include <QApplication>
#include "logger.h"

const QString configPath = CLIENT_CONFIG_FILE_PATH;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Logger::initFromConfig(configPath);
    MainWindow w;
    w.show();
    return a.exec();
}
