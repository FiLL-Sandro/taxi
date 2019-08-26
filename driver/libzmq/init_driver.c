#include "init_driver.h"

/*
 * init_driver -- creation of private data, connection to the server and registration
 * @argc -- amount of arguments;
 * @argv -- array of arguments;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int init_driver(int argc, char *argv[])
{
	size_t size_opt = sizeof(int);

	if ((pri_data = init_peer(my_host, argc, argv)) == NULL)
	{
		log_debug("FAIL: init_peer: ", NULL);
		goto failure;
	}

	if (connect_to_server(&dri_ctx, &dri_sock) == -1)
	{
		log_debug("FAIL: connect_to_server: ", NULL);
		goto failure;
	}

	zmq_msg_init(&dri_zmq_msg);
	if (registration(&dri_zmq_msg, dri_sock, MSG_AUTH_DRI, pri_data, sizeof(driver_t)) == -1)
	{
		log_debug("FAIL: registration: message transmission are failed", NULL);
		goto failure;
	}
	wrap_recv(&dri_zmq_msg, dri_sock, 0);
	wrap_send(&dri_zmq_msg, dri_sock, 0);
	if ((driver_fd = get_fd_from_msg(&dri_zmq_msg)) == -1)
	{
		log_debug("FAIL: get_fd_from_msg: invalid fd", NULL)
				goto failure;
	}

	dri_fd_poll.fd = driver_fd;
	dri_fd_poll.events = 0;
	dri_fd_poll.revents = 0;

	return 0;
failure:
	return -1;
}
