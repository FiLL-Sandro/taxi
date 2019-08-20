#include "communication.h"

/*
 * recv_message -- receiving message
 * @fd -- source fd;
 *
 * return:
 * non-NULL -- pointer to received message;
 * NULL -- failure;
*/
message_t* recv_message(int fd)
{
	message_t *msg = NULL;
	size_t size_data = 0;
	type_msg_t type;

	if (fd < 0)
	{
		log_debug("FAIL: @fd is negative;(fd: %d)", fd);
		goto ret_f;
	}
	else if (wrap_recv(fd, &type, sizeof(type_msg_t), 0) <= 0)
	{
		log_debug("FAIL: wrap_recv: receiving of message header", NULL);
		goto ret_f;
	}
	else if ((msg = create_message(type, NULL, size_data)) == NULL)
	{
		log_debug("FAIL: create_message: unknown type or memory error;(type: %u)", type);
		goto ret_f;
	}

	size_of_message_data(type, &size_data);
	if (size_data != 0 && wrap_recv(fd, msg->data, size_data, 0) <= 0)
	{
		log_debug("FAIL: wrap_recv: data part receiving", NULL);
		goto failure_with_free;
	}

	goto ret_f;

failure_with_free:
	if (msg != NULL) free(msg), msg = NULL;
ret_f:
	return msg;
}

/*
 * send_message -- sending message to @fd
 * @fd - destination fd;
 * @type -- type of message;
 * @data -- data of message;
 * @size_date -- size of data;
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
*/
int send_message(int fd, type_msg_t type, void *data, size_t size_data)
{
	size_t size_msg = sizeof(message_t) + size_data;
	message_t *msg = create_message(type, data, size_data);

	if (fd < 0 || msg == NULL)
	{
		log_debug("FAIL: fd had invalid value or creating message are failed;\n"
				  "(fd: %d, msg: %p)", fd, (void*)msg);
		goto failure;
	}
	else if (wrap_send(fd, msg, size_msg, 0) == -1)
	{
		log_debug("FAIL: wrap_send: sending message_t are failed;", NULL);
		goto failure;
	}

	if (msg != NULL) free(msg);
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
