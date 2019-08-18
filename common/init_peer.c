#include "init_peer.h"

/*
 * init_peer -- create and insert initialization data for @kind
 * @kind -- kind of host;
 * @argc -- amount of arguments;
 * @argv -- transfered arguments;
 *
 * return:
 * void* -- ptr to init-data;
 * NULL -- failure;
 */
void* init_peer(kind_host_t kind, int argc, char *argv[])
{
	void *ret_f = NULL;
	driver_t tmp_dri;
	passenger_t tmp_pass;
	int i;

	srand((unsigned int)time(NULL));

	if (kind == HOST_DRIVER)
	{
		tmp_dri.stat = DRI_FREE;
		tmp_dri.coord[0] = argc == 3 ? atoi(argv[1])
									 : rand() % RANGE_COORD;

		tmp_dri.coord[1] = argc == 3 ? atoi(argv[2])
									 : rand() % RANGE_COORD;

		ret_f = (driver_t*)malloc(sizeof(driver_t));
		memcpy(ret_f, &tmp_dri, sizeof(driver_t));
		log_debug("SUCCESS: initialization seccess", NULL);
	}
	else if (kind == HOST_PASSENGER)
	{
		tmp_pass.stat = PASS_WAIT;
		if (argc == 5)
		{
			for (i = 0; i < 5 - 1; ++i)
			{
				tmp_pass.route[i] = atoi(argv[i + 1]);
			}
		}
		else
		{
			tmp_pass.route[0] = rand() % RANGE_COORD;
			tmp_pass.route[1] = rand() % RANGE_COORD;

			do
			{
				tmp_pass.route[2] = rand() % RANGE_COORD;
				tmp_pass.route[3] = rand() % RANGE_COORD;
			} while(tmp_pass.route[0] == tmp_pass.route[2]);
		}

		ret_f = (passenger_t*)malloc(sizeof(passenger_t));
		memcpy(ret_f, &tmp_pass, sizeof(passenger_t));
		log_debug("SUCCESS: initialization seccess", NULL);
	}
	else
	{
		log_debug("FAIL: unknown kind;(kind: %u)", kind);
	}

ret_f:
	return ret_f;
}

int connect_to_server(struct sockaddr_in **serv_addr, int *server_fd)
{
	if (serv_addr == NULL || server_fd == NULL)
	{
		log_debug("FAIL: bad arguments;\n"
				  "(serv_addr: %p, server_fd: %p)",
				  (void*)serv_addr, (void*)server_fd);
		goto failure;
	}

	if ((*serv_addr = get_sockaddr_in(SERVER_IP_STR, SERVER_PORT)) == NULL)
	{
		log_debug("FAIL: get_sockaddr_in: getting address of server", NULL);
		goto failure;
	}

	if ((*server_fd = tcp_socket_ip4()) == -1)
	{
		log_debug("FAIL: tcp_socket_ip4:", NULL);
		goto failure;
	}

	if ((wrap_connect(*server_fd, (struct sockaddr*)(*serv_addr),
					  sizeof(struct sockaddr_in))) == -1)
	{
		log_debug("FAIL: wrap_connect: connectind to server are failed", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}

int registration(int server_fd, type_msg_t type, void *pri_data, size_t size_data)
{
	if (type != MSG_AUTH_DRI && type != MSG_AUTH_PASS)
	{
		log_debug("FAIL: unknown type;(type: %u)", type);
		goto failure;
	}

	if ((send_message(server_fd, type, pri_data, size_data)) == -1)
	{
		log_debug("FAIL: send_message: registration are failed", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}
