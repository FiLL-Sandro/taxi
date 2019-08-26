#ifndef SUPER_DB_H
#define SUPER_DB_H

#include "common_data.h"
#include "logging.h"
#include "list.h"
#include "vector.h"
#include "libzmq/wrappers_network.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEST_FD_ZMQ struct{ int dest_fd; zmq_msg_t *zmq_msg; }

extern int add_entry(zmq_msg_t* zmq_msg, kind_host_t kind, void *data);
extern int del_entry(int sfd);
extern passenger_t* set_session(zmq_msg_t* dri_zmq);
extern int change_position(int sfd, int *pos);
extern int set_free_peer(int sfd, int *pos);
extern int change_status(int sfd, kind_host_t kind, void *stat);
extern void* take_dest(int sfd);
extern zmq_msg_t* take_zmq_msg(int sfd);

#endif
