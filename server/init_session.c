#include "init_session.h"

void* init_session(void* arg)
{
	int driver_fd = 0;
	passenger_t *pass = NULL;

	while (1)
	{
		pthread_mutex_lock(&access_to_db);			//MUTEX_LOCK

		pass = set_session(&driver_fd);
		if (pass == NULL)
		{
			pthread_mutex_unlock(&access_to_db);	//MUTEX_UNLOCK
			sleep(1);
			continue;
		}

		if (send_message(driver_fd, MSG_INIT_SESSION,
						 pass, sizeof(passenger_t)) == -1)
		{
			log_debug("FAIL: send_message: ");
			set_free_peer(take_dest_fd(driver_fd), NULL);
			del_entry(driver_fd);
		}

		pthread_mutex_unlock(&access_to_db);		//MUTEX_UNLOCK
	}
}
