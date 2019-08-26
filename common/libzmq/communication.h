#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "common_data.h"
#include "wrappers_network.h"
#include "logging.h"
#include <string.h>

extern message_t* recv_message(zmq_msg_t *zmq_msg, void *sock, int flags);
extern int send_message(zmq_msg_t *zmq_msg, void *sock, type_msg_t type,
						void *data, size_t size_data);
extern message_t* create_message(type_msg_t type, void *data, size_t size_data);
extern int size_of_message_data(type_msg_t type, size_t* res);
extern void free_message_t(void *data, void *hint);
extern int reply_message(zmq_msg_t *zmq_msg, void *sock, message_t *msg);

#endif
