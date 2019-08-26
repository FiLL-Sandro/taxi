#ifndef COMMON_DATA_SERVER_H
#define COMMON_DATA_SERVER_H

#include "common_data.h"
#include "libzmq/wrappers_network.h"
#include "libzmq/communication.h"
#include "logging.h"
#include "poll_oper.h"
#include <sys/queue.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <zmq.h>

typedef enum que_msg_type_e
{
	QUE_NORM		= 1,
	QUE_DEL			= 2
} que_msg_type_t;

typedef struct que_msg
{
	TAILQ_ENTRY(que_msg) navi;
	que_msg_type_t type;
	int sfd;
	zmq_msg_t zmq_msg;
	void *data;
} que_msg_t;

TAILQ_HEAD(th_que_msg, que_msg);

extern void *serv_ctx, *serv_sock;
extern struct th_que_msg que_msg_head;
extern pthread_mutex_t access_to_db;
extern int server_fd;

#endif
