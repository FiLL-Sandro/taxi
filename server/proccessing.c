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
	int driver_fd = 0;
	passenger_t *pass = NULL;
	que_msg_type_t mask = QUE_NORM | QUE_DEL;

	while (1)
	{
		if ((que_msg = take_que_msg(mask)) != NULL)
		{
			switch (que_msg->type)
			{
				case QUE_NORM: proc_que_norm(que_msg); break;
				case QUE_DEL: del_entry(que_msg->sfd); break;
			}

			if (que_msg->data != NULL) free(que_msg->data);
			free(que_msg);
		}

		pass = set_session(&driver_fd);
		if (pass != NULL)
		{
			if (send_message(driver_fd, MSG_INIT_SESSION,
							 pass, sizeof(passenger_t)) == -1)
			{
				log_debug("FAIL: send_message: ");
				set_free_peer(take_dest_fd(driver_fd), NULL);
				del_entry(driver_fd);
			}
		}
	}
}

/*
 * proc_que_norm -- message processing QUE_NORM
 * @que_msg -- target message;
 */
void proc_que_norm(que_msg_t *que_msg)
{
	message_t *msg = que_msg->data;
	size_t size_data;
	int sfd = que_msg->sfd, dfd;
	kind_host_t kind;
	void *data = NULL;

	dfd = take_dest_fd(sfd);
	size_of_message_data(msg->type, &size_data);

	switch (msg->type)
	{
		case MSG_AUTH_DRI: case MSG_AUTH_PASS:
			kind = msg->type == MSG_AUTH_DRI ? HOST_DRIVER : HOST_PASSENGER;
			data = malloc(size_data);
			memcpy(data, msg->data, size_data);
			add_entry(sfd, kind, data);
			break;

		case MSG_ORDER_COMP_DRI:
			if (send_message(dfd, msg->type, msg->data, size_data) == -1)
			{
				log_debug("FAIL: send_message: sending message to passenger for close session", NULL);
				del_entry(dfd);
			}
			set_free_peer(sfd, (int*)msg->data);
			break;

		case MSG_ORDER_COMP_PASS:
			del_entry(sfd);
			close(sfd);
			break;

		case MSG_TRANSIT_FROM_DRI: case MSG_TRANSIT_FROM_PASS:
			kind = msg->type == MSG_TRANSIT_FROM_DRI ? HOST_DRIVER : HOST_PASSENGER;
			change_status(sfd, kind, msg->data);
			if (send_message(dfd, msg->type, msg->data, size_data) == -1)
			{
				log_debug("FAIL: send_message: transit message;(kind: %u)", kind);
				del_entry(sfd), del_entry(dfd);
			}
			break;

		default:
			close(sfd);
			break;
	}
}

//________________________________________
