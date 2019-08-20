#ifndef COMMON_DATA_H
#define COMMON_DATA_H

#include <netinet/in.h>

#define SERVER_IP_DIGI INADDR_LOOPBACK
#define SERVER_IP_STR "127.0.0.1"
#define SERVER_PORT 50001

#include <stdlib.h>

typedef enum type_msg_e
{
	MSG_AUTH_DRI = 1,
	MSG_AUTH_PASS,
	MSG_ORDER_COMP_DRI,
	MSG_ORDER_COMP_PASS,
	MSG_TRANSIT_FROM_DRI,
	MSG_TRANSIT_FROM_PASS,
	MSG_INIT_SESSION,
	MSG_DRI_CUR_POS
} type_msg_t;

typedef enum p_stat_e
{
	PASS_WAIT = 0,
	PASS_LOAD,
	PASS_RIDE_TO,
	PASS_COMP,
	PASS_IGNOR
} p_stat_t;

typedef enum d_stat_e
{
	DRI_FREE = 0,
	DRI_RIDE_TO,
	DRI_ON_SIDE,
	DRI_IMP_ORD,
	DRI_COMP,
	DRI_IGNOR
} d_stat_t;

typedef struct passenger
{
	int route[4];
	int time_load;
	p_stat_t stat;
} passenger_t;

typedef struct driver
{
	int coord[2];
	int speed;
	d_stat_t stat;
} driver_t;

typedef struct message
{
	type_msg_t type;
	u_char data[];
} message_t;

typedef enum kind_host_e
{
	HOST_DRIVER = 1,
	HOST_PASSENGER,
	HOST_SERVER
} kind_host_t;

#endif
