#ifndef COMMON_DATA_PASSENGER_H
#define COMMON_DATA_PASSENGER_H

#include "common_data.h"
#include "libc/communication.h"
#include "libc/wrappers_network.h"
#include "logging.h"
#include <stdbool.h>

#define RESP_STAT 5

extern kind_host_t my_host;
extern struct sockaddr_in *serv_addr;
extern int sfd;
extern passenger_t *pri_data;
extern p_stat_t *resp_arr[RESP_STAT];
extern bool flag_complete_work;


#endif
