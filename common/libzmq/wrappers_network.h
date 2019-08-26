#ifndef WRAPPERS_NETWORK_H
#define WRAPPERS_NETWORK_H

#include <stdio.h>
#include <stdlib.h>

#include <zmq.h>
#include "logging.h"

typedef enum type_sock_e
{
	TYPE_SERVER = ZMQ_REP,
	TYPE_CLIENT = ZMQ_REQ
} type_sock_t;

extern void* wrap_socket(void** ctx, type_sock_t type_sock);
extern int wrap_bind(void* sock, const char* endpoint);
extern int wrap_connect(void *sock, const char *endpoint);
extern int wrap_recv(zmq_msg_t *msg, void *socket, int flags);
extern int wrap_send(zmq_msg_t *msg, void *socket, int flags);

extern int wrap_init_data(zmq_msg_t *msg, void *data, size_t size, zmq_free_fn* ffn);
extern int get_fd_from_msg(zmq_msg_t *msg);

#endif
