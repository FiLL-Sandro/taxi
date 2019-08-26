#include "proccessing.h"

static void proc_que_norm(que_msg_t *que_msg);

/*
 * proccessing -- proccessing message of queue
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
 */
void* proccessing(void *arg)
{
	que_msg_t *que_msg = NULL;
	que_msg_type_t mask = QUE_NORM | QUE_DEL;

	while (1)
	{
		if ((que_msg = take_que_msg(mask)) == NULL)
		{
			sleep(1);
			continue;
		}

		pthread_mutex_lock(&access_to_db);			//MUTEX_LOCK

		switch (que_msg->type)
		{
			case QUE_NORM: proc_que_norm(que_msg); break;
			case QUE_DEL: del_entry(que_msg->sfd); break;
		}

		pthread_mutex_unlock(&access_to_db);		//MUTEX_UNLOCK

		free_que_msg(que_msg);
	}
}

/*
 * proc_que_norm -- message processing QUE_NORM
 * @que_msg -- target message;
 */
void proc_que_norm(que_msg_t *que_msg)
{
	int sfd = que_msg->sfd;
	zmq_msg_t zmq_msg;
	message_t *msg = que_msg->data;

	void *data = NULL;
	size_t size_data;
	kind_host_t kind;
	DEST_FD_ZMQ *dfd = NULL;

	dfd = take_dest(sfd);
	size_of_message_data(msg->type, &size_data);

	switch (msg->type)
	{
		case MSG_AUTH_DRI: case MSG_AUTH_PASS:
			kind = msg->type == MSG_AUTH_DRI ? HOST_DRIVER : HOST_PASSENGER;
			data = malloc(size_data);
			zmq_msg = que_msg->zmq_msg;

			memcpy(data, msg->data, size_data);

			add_entry(&zmq_msg, kind, data);

			wrap_send(&zmq_msg, serv_sock, 0);
			wrap_recv(&zmq_msg, serv_sock, 0);
			break;

		case MSG_ORDER_COMP_DRI:
			if (send_message(dfd->zmq_msg, serv_sock, msg->type,
							 msg->data, size_data) == -1)
			{
				log_debug("FAIL: send_message: "
						  "sending message to passenger for close session", NULL);
				del_entry(dfd->dest_fd);
			}
			set_free_peer(sfd, (int*)msg->data);
			break;

		case MSG_ORDER_COMP_PASS:
			zmq_msg = *(take_zmq_msg(sfd));
			reply_message(&zmq_msg, serv_sock, msg);

			del_entry(sfd);
			break;

		case MSG_TRANSIT_FROM_DRI: case MSG_TRANSIT_FROM_PASS:
			kind = msg->type == MSG_TRANSIT_FROM_DRI ? HOST_DRIVER : HOST_PASSENGER;

			change_status(sfd, kind, msg->data);
			if (send_message(dfd->zmq_msg, serv_sock,
							 msg->type, msg->data, size_data) == -1)
			{
				log_debug("FAIL: send_message: transit message;(kind: %u)", kind);
				del_entry(sfd), del_entry(dfd->dest_fd);
			}
			break;

		case MSG_DRI_CUR_POS:
			zmq_msg = *(take_zmq_msg(sfd));

			change_position(sfd, (int*)msg->data);
			reply_message(&zmq_msg, serv_sock, msg);
			break;
		default:
			del_entry(que_msg->sfd);
			break;
	}
}

//________________________________________
