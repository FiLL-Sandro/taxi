#include "server.h"

int listen_sfd;
struct sockaddr_in *server_addr = NULL;
struct th_que_msg que_msg_head =
		TAILQ_HEAD_INITIALIZER(que_msg_head);

int main(void)
{
	int i = 0;

	if (init_server() == -1) goto failure;

	for (; ; )
	{
		if (receiving() == -1) goto failure;
		if (proccessing() == -1) goto failure;
	}


	exit(EXIT_SUCCESS);
failure:
	exit(EXIT_FAILURE);
}
