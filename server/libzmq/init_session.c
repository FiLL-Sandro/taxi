#include "init_session.h"

void* init_session(void* arg)
{
	zmq_msg_t dri_zmq_msg;
	passenger_t *pass = NULL;
	DEST_FD_ZMQ *dest = NULL;
	int sfd = 0;

	while (1)
	{
		pthread_mutex_lock(&access_to_db);			//MUTEX_LOCK

		pass = set_session(&dri_zmq_msg);
		if (pass == NULL)
		{
			pthread_mutex_unlock(&access_to_db);	//MUTEX_UNLOCK
			sleep(1);
			continue;
		}

		sfd = get_fd_from_msg(&dri_zmq_msg);
		dest = take_dest(sfd);
		if (send_message(&dri_zmq_msg, serv_sock, MSG_INIT_SESSION,
						 pass, sizeof(passenger_t)) == -1)
		{
			log_debug("FAIL: send_message: ", NULL);
			set_free_peer(dest->dest_fd, NULL);
			del_entry(sfd);
		}

		pthread_mutex_unlock(&access_to_db);		//MUTEX_UNLOCK
	}
}
