#ifndef RABBITSERVER_H
#define RABBITSERVER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <assert.h>
#include "utils.h"
#include "logger.h"
#include "Messages.pb.h"

class RabbitServer
{
public:
    RabbitServer();
    void initFromConfig(const QString& configFilePath);
    void start();
    TestTask::Messages::Response doubleReq(TestTask::Messages::Request request);
    QString getHostname();
    int getPort();
    QString getExchange();
    QString getBingkey();

private:
    QString hostname;
    int port;
    QString exchange;
    QString bindingkey;
    amqp_connection_state_t conn;
    amqp_socket_t *socket;
};

#endif // RABBITSERVER_H
