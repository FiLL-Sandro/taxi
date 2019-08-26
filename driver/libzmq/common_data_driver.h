#ifndef COMMON_DATA_DRIVER_H
#define COMMON_DATA_DRIVER_H

#include "common_data.h"
#include "libzmq/communication.h"
#include "libzmq/wrappers_network.h"
#include "logging.h"
#include <signal.h>
#include <unistd.h>
#include <zmq.h>
#include <poll.h>

#define RESP_STAT 4

extern kind_host_t my_host;
extern driver_t *pri_data;
extern int *route;
extern d_stat_t *resp_arr[RESP_STAT];
extern void *dri_ctx, *dri_sock;
extern zmq_msg_t dri_zmq_msg;
extern int driver_fd;
extern struct pollfd dri_fd_poll;

#endif
