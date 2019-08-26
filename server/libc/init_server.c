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

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

	sigact.sa_mask = mask;
	sigact.sa_handler = sigint_hand;
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);

	listen_sfd = tcp_socket_ip4();
	if (listen_sfd < 0) goto failure;

	server_addr = get_sockaddr_in(SERVER_IP_STR, SERVER_PORT);
	if (server_addr == NULL) goto failure;

	if (wrap_bind(listen_sfd, (struct sockaddr*)(server_addr),
				  sizeof(struct sockaddr_in)) == -1) goto failure;

	if (wrap_listen(listen_sfd, 5) == -1) goto failure;

	if (add_pollfd(listen_sfd, POLLIN) == -1) goto failure;

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

	close(listen_sfd);
}
//________________________________________________________________

