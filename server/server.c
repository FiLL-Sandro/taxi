#include "server.h"

int listen_sfd;
struct sockaddr_in *server_addr = NULL;
struct th_que_msg que_msg_head =
		TAILQ_HEAD_INITIALIZER(que_msg_head);

int main(void)
{
	int i = 0;
	pthread_t tid_main = pthread_self();
	pthread_t tid_proc, tid_recv;

	if (init_server() == -1) goto failure;
	pthread_create(&tid_recv, NULL, receiving, &tid_main);
	pthread_create(&tid_proc, NULL, proccessing, NULL);

	pause();
failure:
	exit(EXIT_FAILURE);
}
