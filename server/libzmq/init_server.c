#include "init_server.h"

static void sigint_hand(int sig);

/*
 * init_server -- initialization server;
 *
 * return:
 * 0 -- success;
 * -1 -- failure
 */
int init_server(void)
{
	sigset_t mask;
	struct sigaction sigact;
	size_t opt_size = sizeof(int);

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
	pthread_sigmask(SIG_BLOCK, NULL, &mask);

	sigact.sa_mask = mask;
	sigact.sa_handler = sigint_hand;
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);

	if ((serv_sock = wrap_socket(&serv_ctx, TYPE_SERVER)) == NULL) goto failure;

	if (wrap_bind(serv_sock, "tcp://" SERVER_IP_STR ":" SERVER_PORT_STR) == -1) goto failure;

	if (zmq_getsockopt(serv_sock, ZMQ_FD, &server_fd, &opt_size) == -1)
	{
		log_debug("FAIL: zmq_getsockopt: %s", zmq_strerror(errno));
		goto failure;
	}

	add_pollfd(server_fd, 0);

	TAILQ_INIT(&que_msg_head);

	goto success;

failure:
	log_info("Initializations are failed...", NULL);
	return -1;
success:
	log_info("Initialization success...", NULL);
	return 0;
}

//static_functions________________________________________________
/*
 * sigint_hand -- SIGINT handler;
 * @sig -- signal number;
 */
void sigint_hand(int sig)
{
	log_info("SIGINT was caught", NULL);

	exit(EXIT_FAILURE);
}
//________________________________________________________________

