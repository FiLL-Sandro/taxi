#ifndef INIT_PEER_H
#define INIT_PEER_H

#include "common_data.h"
#include "logging.h"
#include "wrappers_network.h"
#include "communication.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define RANGE_COORD 100
#define LEFT_BORDER_SPEED 10
#define DELTA_SPEED 5
#define DELTA_TIME_LOAD 5

extern void* init_peer(kind_host_t kind, int argc, char *argv[]);
extern int connect_to_server(struct sockaddr_in **serv_addr, int *server_fd);
extern int registration(int server_fd, type_msg_t type, void *pri_data, size_t size_data);

#endif
