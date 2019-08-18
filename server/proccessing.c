#include "proccessing.h"

static que_msg_t* check_que_msg(void);
static void proc_que_norm(que_msg_t *que_msg);
static void proc_que_del(que_msg_t *que_msg);
static void delete_sfd(int sfd);

/*
 * proccessing -- proccessing message of queue
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
 */
int proccessing(void)
{
	que_msg_t *que_msg = NULL;
	int driver_fd = 0, passenger_fd = 0, i = 0;
	passenger_t *pass = NULL;

	while ((que_msg = check_que_msg()) != NULL)
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
			delete_sfd(driver_fd);
		}
	}

	return 0;
}

//static_functions________________________
/*
 * check_que_msg -- message check;
 *
 * return:
 * non-NULL -- valid message;
 * NULL -- invalid message;
 */
que_msg_t* check_que_msg(void)
{
	que_msg_t *que_msg = NULL;

	if (!TAILQ_EMPTY(&que_msg_head))
	{
		que_msg = TAILQ_FIRST(&que_msg_head);
		if (que_msg->type != QUE_POLL_DEL)
		{
			TAILQ_REMOVE(&que_msg_head, que_msg, navi);
		}
		else
		{
			que_msg = NULL;
		}
	}

	return  que_msg;
}

/*
 * proc_que_norm -- message processing QUE_NORM
 * @que_msg -- target message;
 */
void proc_que_norm(que_msg_t *que_msg)
{
	message_t *msg = que_msg->data;
	size_t size_data;
	int sfd = que_msg->sfd, dfd = take_dest_fd(sfd);
	kind_host_t kind;
	void *data = NULL;

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
				delete_sfd(dfd);
			}
			set_free_peer(sfd, (int*)msg->data);
			break;

		case MSG_ORDER_COMP_PASS:
			delete_sfd(sfd);
			break;

		case MSG_TRANSIT_FROM_DRI: case MSG_TRANSIT_FROM_PASS:
			kind = msg->type == MSG_TRANSIT_FROM_DRI ? HOST_DRIVER : HOST_PASSENGER;
			change_status(sfd, kind, msg->data);
			if (send_message(dfd, msg->type, msg->data, size_data) == -1)
			{
				log_debug("FAIL: send_message: transit message;(kind: %u)", kind);
				delete_sfd(sfd), delete_sfd(dfd);
			}
			break;

		default:
			delete_sfd(sfd);
			break;
	}
}

/*
 * delete_sfd -- delete a record, close the connection and request to delete poll_fd
 * @sfd -- cell/socket
 */
void delete_sfd(int sfd)
{
	que_msg_t *que_msg = NULL;

	del_entry(sfd);

	que_msg = (que_msg_t*)malloc(sizeof(que_msg_t));
	que_msg->type = QUE_POLL_DEL, que_msg->sfd = sfd;
	TAILQ_INSERT_HEAD(&que_msg_head, que_msg, navi);
}

//________________________________________
