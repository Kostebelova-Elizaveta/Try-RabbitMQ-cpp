#include "utils.h"

void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    QString message;
    message.vsprintf(fmt, ap);
    Logger::log(ERROR, message);
    va_end(ap);
    exit(1);
}


void die_on_error(int x, char const *context) {
    if (x < 0) {
        Logger::log(ERROR,  QString("%1: %2").arg(context).arg(amqp_error_string2(x)));
        exit(1);
    }
}

void die_on_amqp_error(amqp_rpc_reply_t x, char const *context) {
    switch (x.reply_type) {
    case AMQP_RESPONSE_NORMAL:
        return;

    case AMQP_RESPONSE_NONE:
        Logger::log(ERROR, QString("%1: missing RPC reply type!").arg(context));
        break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        Logger::log(ERROR, QString("%1: %2").arg(context).arg(amqp_error_string2(x.library_error)));
        break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
            amqp_connection_close_t *m =
                    (amqp_connection_close_t *)x.reply.decoded;
            Logger::log(ERROR, QString("%1: server connection error %2h, message: %3")
                        .arg(context)
                        .arg(m->reply_code)
                        .arg(QString::fromLatin1((char *)m->reply_text.bytes, m->reply_text.len)));
            break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD: {
            amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
            Logger::log(ERROR, QString("%1: server channel error %2h, message: %3")
                        .arg(context)
                        .arg(m->reply_code)
                        .arg(QString::fromLatin1((char *)m->reply_text.bytes, m->reply_text.len)));
            break;
        }
        default:
            Logger::log(ERROR, QString("%1: unknown server error, method id 0x%2X").arg(context).arg(x.reply.id, 8, 16, QLatin1Char('0')));
            break;
        }
        break;
    }
    exit(1);
}
