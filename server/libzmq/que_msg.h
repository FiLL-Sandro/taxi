#ifndef QUE_MSG_H
#define QUE_MSG_H

#include "common_data_server.h"

extern que_msg_t* take_que_msg(que_msg_type_t mask);
extern void insert_to_que_msg(que_msg_t* que_msg);
extern void free_que_msg(que_msg_t *que_msg);
extern que_msg_t* crt_set_que_msg(que_msg_type_t type, int sfd,
								  zmq_msg_t *zmq_msg, void *data);

#endif
