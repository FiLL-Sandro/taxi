#include "init_peer.h"

static void sigalrm_hand(int sig);
static void setting_signal(void);

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

	setting_signal();

	srand((unsigned int)time(NULL));

	if (kind == HOST_DRIVER)
	{
		tmp_dri.speed = LEFT_BORDER_SPEED + rand() % DELTA_SPEED;
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
		tmp_pass.time_load = rand() % DELTA_TIME_LOAD;
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

/*
 * connect_to_server -- connecting to server and create socket and context
 * @ctx -- context ZMQ;
 * @sock -- socket;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int connect_to_server(void **ctx, void **sock)
{
	if (ctx == NULL || sock == NULL)
	{
		log_debug("FAIL: ctx == NULL || sock == NULL;\n"
				  "(ctx: %p, sock: %p)", (void*)ctx, (void*)sock);
		goto failure;
	}
	else if ((*sock = wrap_socket(ctx, TYPE_CLIENT)) == NULL)
	{
		log_debug("FAIL: wrap_socket: error with socket creating", NULL);
		goto failure_with_free;
	}
	else if (wrap_connect(*sock, "tcp://" SERVER_IP_STR ":" SERVER_PORT_STR) == -1)
	{
		log_debug("FAIL: wrap_connect: connectind to server are failed", NULL);
		goto failure_with_free;
	}

	return 0;
failure_with_free:
	if (*sock != NULL) zmq_close(*sock), *sock = NULL;
	if (*ctx != NULL) zmq_ctx_destroy(*ctx), *ctx = NULL;
failure:
	return -1;
}

/*
 * registration -- sending message for registration on server
 * @zmq_msg -- buffer for sending message;
 * @sock -- socket;
 * @type -- message type;
 * @pri_data -- private data of peer;
 * @size_data -- size of private data;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int registration(zmq_msg_t *zmq_msg, void *sock, type_msg_t type,
				 void *pri_data, size_t size_data)
{
	if (type != MSG_AUTH_DRI && type != MSG_AUTH_PASS)
	{
		log_debug("FAIL: unknown type;(type: %u)", type);
		goto failure;
	}
	else if ((send_message(zmq_msg, sock, type, pri_data, size_data)) == -1)
	{
		log_debug("FAIL: send_message: registration are failed", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}

//static_functions____________________________________________
/*
 * sigalrm_hand -- SIGALRM handler
 * @sig -- signal number;
 */
void sigalrm_hand(int sig)
{
	log_info("SIGALRM: server no responce", NULL);
	exit(EXIT_FAILURE);
}

/*
 * setting_signal -- settings for processing signal SIGALRM
 */
void setting_signal()
{
	sigset_t mask;
	struct sigaction sigact;

	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	sigprocmask(SIG_UNBLOCK, NULL, &mask);

	sigact.sa_mask = mask;
	sigact.sa_handler = sigalrm_hand;
	sigact.sa_flags = 0;
	sigaction(SIGALRM, &sigact, NULL);
}
//____________________________________________________________
