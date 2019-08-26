#ifndef COMMON_DATA_PASSENGER_H
#define COMMON_DATA_PASSENGER_H

#include "common_data.h"
#include "libzmq/communication.h"
#include "libzmq/wrappers_network.h"
#include "logging.h"
#include <stdbool.h>
#include <poll.h>

#define RESP_STAT 5

extern zmq_msg_t pass_zmq_msg;
extern struct pollfd pass_poll;
extern void *pass_sock, *pass_ctx;
extern int passenger_fd;
extern kind_host_t my_host;
extern passenger_t *pri_data;
extern p_stat_t *resp_arr[RESP_STAT];
extern bool flag_complete_work;


#endif
