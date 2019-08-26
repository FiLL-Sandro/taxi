#include "wrappers_network.h"

/* ##################################################
 * wrap_socket -- create new ØMQ context and ØMQ socket
 * @ctx -- pointer to variable storages context
 * @type -- socket type
 *
 * return:
 * NULL -- failure;
 * non-NULL -- created socket
 */

void* wrap_socket(void **ctx, type_sock_t type_sock)
{
	void *sock = NULL;

	if (ctx == NULL)
	{
		log_debug("FAIL: ctx == NULL;\n", NULL);
		goto failure;
	}
	else if ((*ctx = zmq_ctx_new()) == NULL)
	{
		log_debug("FAIL: zmq_ctx_new: %s", zmq_strerror(errno));
		goto failure;
	}
	else if ((sock = zmq_socket(*ctx, type_sock)) == NULL)
	{
		log_debug("FAIL: zmq_socket: %s", zmq_strerror(errno));
		goto failure;
	}

	return sock;
failure:
	return NULL;
}

// ##################################################

/*
 * wrap_bind -- accept incoming connections on a socket
 * @sock -- pointer to socket
 * @endpoint -- address:port
 *
 * return:
 *		0 -- success; -1 -- failed;
 */

int wrap_bind(void *sock, const char *endpoint)
{
	int ret_f = zmq_bind(sock, endpoint);

	if (ret_f == -1)
	{
		log_debug("FAIL: zmq_bind: %s", zmq_strerror(errno));
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_connect -- create outgoing connection from socket
 * @sock -- pointer to socket
 * @endpoint -- address:port
 *
 * return:
 *		0 -- success; -1 -- failed;
 */

int wrap_connect(void *sock, const char *endpoint)
{
	int ret_f = zmq_connect(sock, endpoint);

	if (ret_f == -1)
	{
		log_debug("FAIL: zmq_connect: %s", zmq_strerror(errno));
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_recv -- receive a message part from a socket
 * @msg -- pointer to zmq_msg_t which is storage for receiving message
 * @socket -- target socket
 * @flags -- flags
 *
 * return:
 * <= 0 -- failure;
 * > 0 -- success: number of received bytes;
 */

int wrap_recv(zmq_msg_t *msg, void *socket, int flags)
{
	int ret_f = -1;

	if (msg == NULL || socket == NULL)
	{
		log_debug("FAIL: msg == NULL || socket == NULL;\n"
				  "(msg: %p, socket: %p)", (void*)msg, (void*)socket);
		goto ret_f;
	}
	else if ((ret_f = zmq_msg_recv(msg, socket, flags)) <= 0)
	{
		log_debug("FAIL: zmq_msg_recv: connection closed of error\n"
				  "(errno: %s)", zmq_strerror(errno));
	}

ret_f:
	return ret_f;
}

// ##################################################

/*
 * wrap_send -- send a message part on a socket
 * @msg -- pointer to message for sending
 * @socket -- target socket
 * @flags -- flags
 *
 * return:
 * -1 -- failure;
 * > -1 -- success: number of sent bytes;
 */

int wrap_send(zmq_msg_t *msg, void *socket, int flags)
{
	int ret_f = -1;

	if (msg == NULL || socket == NULL)
	{
		log_debug("FAIL: msg == NULL || socket == NULL;\n"
				  "(msg: %p, socket: %p)", (void*)msg, (void*)socket);
		goto ret_f;
	}
	else if ((ret_f = zmq_msg_send(msg, socket, flags)) == -1)
	{
		log_debug("FAIL: zmq_msg_send: %s", zmq_strerror(errno));
	}

ret_f:
	return ret_f;
}

// ##################################################

/*
 * wrap_init_data -- initialise ØMQ message from a supplied buffer
 * @msg -- pointer to target message;
 * @data -- data for message;
 * @size -- size of data;
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
 */
int wrap_init_data(zmq_msg_t *msg, void *data, size_t size, zmq_free_fn *ffn)
{
	int ret_f = -1;

	if (msg == NULL || data == NULL)
	{
		log_debug("FAIL: msg == NULL || data == NULL;\n"
				  "(msg: %p, data: %p)", (void*)msg, (void*)data);
		goto ret_f;
	}

	ret_f = zmq_msg_init_data(msg, data, size, ffn, NULL);

	if (ret_f == -1)
	{
		log_debug("FAIL: zmq_msg_init_data: %s", zmq_strerror(errno));
	}

ret_f:
	return ret_f;
}

// ##################################################

/*
 * get_fd_from_msg -- getting fd from message
 * @msg -- pointer to target message;
 *
 * return:
 * -1 -- failure;
 * > -1 -- success: file descriptor;
 */
int get_fd_from_msg(zmq_msg_t *msg)
{
	int ret_f = -1;

	if (msg == NULL)
	{
		log_debug("FAIL: msg == NULL", NULL);
		goto ret_f;
	}
	ret_f = zmq_msg_get(msg, ZMQ_SRCFD);

ret_f:
	return ret_f;
}

// ##################################################
