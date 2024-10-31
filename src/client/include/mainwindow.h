#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QDebug>
#include "Messages.pb.h"
#include "logger.h"
#include <iostream>
#include <QMessageBox>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include "dialogtoeditsettings.h"
#include "utils.h"
#include "queuelistener.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void startListening();

private slots:
    void on_sendButton_clicked();

    void on_editSettingsButton_clicked();

    void reloadSettings(QSettings *settings);

    void onMessageReceived(QString num);

private:
    Ui::MainWindow *ui;
    TestTask::Messages::Request request;
    QString hostname;
    int port, status;
    QString exchange;
    QString routingkey;
    DialogToEditSettings *dialog;
    amqp_bytes_t queueName;
    amqp_socket_t *socket;
    amqp_connection_state_t conn;
    QueueListener *listenThread;

    std::string getRandomString();
};
#endif // MAINWINDOW_H
