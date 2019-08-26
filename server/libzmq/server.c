#include "server.h"

void *serv_ctx = NULL, *serv_sock = NULL;
int server_fd = 0;
pthread_mutex_t access_to_db = PTHREAD_MUTEX_INITIALIZER;
struct th_que_msg que_msg_head =
		TAILQ_HEAD_INITIALIZER(que_msg_head);

int main(void)
{
	int i = 0;
	pthread_t tid_main = pthread_self();
	pthread_t tid_proc[THREADS], tid_recv, tid_sess;

	if (init_server() == -1) goto failure;
	pthread_create(&tid_recv, NULL, receiving, &tid_main);
	for (i = 0; i < THREADS; ++i)
	{
		pthread_create(&tid_proc[i], NULL, proccessing, NULL);
	}
	pthread_create(&tid_sess, NULL, init_session, NULL);

	pause();
failure:
	log_info("SERVER CRASHED", NULL);
	exit(EXIT_FAILURE);
}
