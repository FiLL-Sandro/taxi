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
	if ((pri_data = init_peer(my_host, argc, argv)) == NULL)
	{
		log_debug("FAIL: init_peer: ", NULL);
		goto failure;
	}

	if (connect_to_server(&serv_addr, &sfd) == -1)
	{
		log_debug("FAIL: connect_to_server: ");
		goto failure;
	}

	if (registration(sfd, MSG_AUTH_DRI, pri_data, sizeof(driver_t)) == -1)
	{
		log_debug("FAIL: registration: message transmission are failed", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}
