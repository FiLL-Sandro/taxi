#include "communication.h"

/*
 * recv_message -- receiving message
 * @zmq_msg -- buffer into which will received message
 * @sock -- target socket
 *
 * return:
 * non-NULL -- success: pointer to received message;
 * NULL -- failure;
*/
message_t* recv_message(zmq_msg_t *zmq_msg, void *sock, int flags)
{
	message_t *msg = NULL;
	type_msg_t type;
	int size_msg = -1;

	if (zmq_msg == NULL || sock == NULL)
	{
		log_debug("FAIL: zmq_msg == NULL || sock == NULL;\n"
				  "(zmq_msg: %p, sock: %p)", (void*)zmq_msg, (void*)sock);
		goto ret_f;
	}
	else if ((size_msg = wrap_recv(zmq_msg, sock, flags)) <= 0)
	{
		log_debug("FAIL: wrap_recv: connection interrupted or error", NULL);
		goto ret_f;
	}

	msg = (message_t*)malloc((size_t)size_msg);
	memcpy(msg, zmq_msg_data(zmq_msg), (size_t)size_msg);

ret_f:
	return msg;
}

/*
 * send_message -- sending message
 * @zmq_msg -- buffer for storage message;
 * @sock -- target socket;
 * @type -- type of message;
 * @data -- data of message;
 * @size_date -- size of data;
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
*/

int reply_message(zmq_msg_t *zmq_msg, void *sock, message_t *msg)
{
	message_t *rpl = NULL;

	if (zmq_msg == NULL || sock == NULL || msg == NULL)
	{
		log_debug("FAIL: zmq_msg == NULL || sock == NULL || msg == NULL;\n"
				  "(zmq_msg: %p, sock: %p, msg: %p)",
				  (void*)zmq_msg, (void*)sock, (void*)msg);
		goto failure;
	}

	rpl = (message_t*)malloc(sizeof(message_t));
	rpl->type = msg->type;

	if (wrap_init_data(zmq_msg, rpl, sizeof(message_t), free_message_t) == -1)
	{
		log_debug("FAIL: wrap_init_data: initialization are failed", NULL);
		goto failure;
	}
	else if (wrap_send(zmq_msg, sock, 0) == -1)
	{
		log_debug("FAIL: wrap_send: reply sending are failed", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}

int send_message(zmq_msg_t *zmq_msg, void *sock, type_msg_t type,
				 void *data, size_t size_data)
{
	size_t size_msg = sizeof(message_t) + size_data;
	message_t *msg = create_message(type, data, size_data);

	if (zmq_msg == NULL || msg == NULL)
	{
		log_debug("FAIL: zmq_msg == NULL || msg == NULL;\n"
				  "(fd: %p, msg: %p)", (void*)zmq_msg, (void*)msg);
		goto failure;
	}
	else if (wrap_init_data(zmq_msg, msg, size_msg, free_message_t) == -1)
	{
		log_debug("FAIL: wrap_init_data: initialization are failed", NULL);
		goto failure;
	}
	else if (wrap_send(zmq_msg, sock, 0) == -1)
	{
		log_debug("FAIL: wrap_send: sending message_t are failed;", NULL);
		goto failure;
	}

	return  0;
failure:
	if (msg != NULL) free(msg);
	return -1;
}

/*
 * create_message -- creates a message with the given arguments
 * @type -- type of message;
 * @data -- data of message;
 * @size_date -- size of data;
 *
 * return:
 * NULL -- failure;
 * non-NULL -- pointer to created message_t
 */
message_t* create_message(type_msg_t type, void *data, size_t size_data)
{
	size_t msg_size = 0;
	message_t *msg = NULL;

	if (size_of_message_data(type, &msg_size) == -1)
	{
		log_debug("FAIL: unknown type;(type: %u)", type);
		goto ret_f;
	}
	else if (data != NULL && size_data != msg_size)
	{
		log_debug("FAIL: invalid size of @data", NULL);
		goto ret_f;
	}

	msg_size += sizeof(message_t);
	if ((msg = (message_t*)malloc(msg_size)) == NULL)
	{
		log_err_with_perror("FAIL: malloc: memory for new message", NULL);
		goto ret_f;
	}

	msg->type = type;
	if (data != NULL) memcpy(msg->data, data, size_data);

ret_f:
	return msg;
}

/*
 * size_of_message_data -- data size according to message type
 * @type -- type of message;
 * @res -- value-result(data size);
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int size_of_message_data(type_msg_t type, size_t *res)
{
	if (res == NULL)
	{
		log_debug("FAIL: no place for result;(res: %p)", (void*)res);
		goto failure;
	}

	switch (type)
	{
		case MSG_AUTH_DRI: *res = sizeof(driver_t); break;
		case MSG_AUTH_PASS: *res = sizeof(passenger_t); break;
		case MSG_ORDER_COMP_DRI: *res = sizeof(int) * 2; break;
		case MSG_ORDER_COMP_PASS: *res = 0; break;
		case MSG_TRANSIT_FROM_DRI: *res = sizeof(d_stat_t); break;
		case MSG_TRANSIT_FROM_PASS: *res = sizeof(p_stat_t); break;
		case MSG_INIT_SESSION: *res = sizeof(passenger_t); break;
		case MSG_DRI_CUR_POS: *res = sizeof(int) * 2; break;
		default:
			log_debug("FAIL: unknown type;(type: %u)", type);
			goto failure;
	}

	return 0;
failure:
	return -1;
}

/*
 * free_message_t -- function for deallocation message_t
 * @data -- message_t;
 * @hint -- always equal NULL;
 */
void free_message_t(void *data, void *hint)
{
	if (data != NULL) free(data);
}
