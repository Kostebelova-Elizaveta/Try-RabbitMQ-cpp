#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/framing.h>
#include "logger.h"
#include <cstdlib>

void die(const char *fmt, ...);
extern void die_on_error(int x, char const *context);
extern void die_on_amqp_error(amqp_rpc_reply_t x, char const *context);

#endif // UTILS_H
