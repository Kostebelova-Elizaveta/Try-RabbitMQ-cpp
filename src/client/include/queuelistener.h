#ifndef QUEUELISTENER_H
#define QUEUELISTENER_H

#include <QThread>
#include <QMainWindow>
#include "utils.h"
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include "Messages.pb.h"
#include <QDebug>
#include "logger.h"


class QueueListener : public QThread
{
    Q_OBJECT

public:
    QueueListener(amqp_connection_state_t &conn, QString &exchange, QString id);

protected:
    void run() override;

signals:
    void messageReceived(QString num);

private:
    amqp_connection_state_t &m_conn;
    QString &m_exchange;
    QString m_id;
};


#endif // QUEUELISTENER_H
