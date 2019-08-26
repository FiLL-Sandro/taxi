#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "common_data.h"
#include "wrappers_network.h"
#include "logging.h"
#include <string.h>

extern message_t* recv_message(int fd);
extern int send_message(int fd, type_msg_t type, void* data, size_t size_data);
extern message_t* create_message(type_msg_t type, void *data, size_t size_data);
extern int size_of_message_data(type_msg_t type, size_t* res);

#endif
