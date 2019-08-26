#include "init_pass.h"

/*
 * init_pass -- creation of private data, connection to the server and registration
 * @argc -- amount of arguments;
 * @argv -- array of arguments;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int init_pass(int argc, char *argv[])
{
	if ((pri_data = init_peer(my_host, argc, argv)) == NULL)
	{
		log_debug("FAIL: init_peer: ", NULL);
		goto failure;
	}

	if (connect_to_server(&pass_sock, &pass_ctx) == -1)
	{
		log_debug("FAIL: connect_to_server: ");
		goto failure;
	}

	zmq_msg_init(&pass_zmq_msg);
	if (registration(&pass_zmq_msg, pass_sock, MSG_AUTH_PASS,
					 pri_data, sizeof(passenger_t)) == -1)
	{
		log_debug("FAIL: registration: message transmission are failed", NULL);
		goto failure;
	}

	wrap_recv(&pass_zmq_msg, pass_sock, 0);
	if ((passenger_fd = get_fd_from_msg(&pass_zmq_msg)) == -1)
	{
		log_debug("FAIL: get_fd_from_msg: invalid fd", NULL)
				goto failure;
	}

	pass_poll.fd = passenger_fd;
	pass_poll.events = 0;
	pass_poll.revents = 0;

	return 0;
failure:
	return -1;
}
