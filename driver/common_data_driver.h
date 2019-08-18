#ifndef COMMON_DATA_DRIVER_H
#define COMMON_DATA_DRIVER_H

#include "common_data.h"
#include "communication.h"
#include "wrappers_network.h"
#include "logging.h"

#define RESP_STAT 4

extern kind_host_t my_host;
extern struct sockaddr_in *serv_addr;
extern int sfd;
extern driver_t *pri_data;
extern int *route;
extern d_stat_t *resp_arr[RESP_STAT];

#endif
