#include "receiving.h"

static int proc_res_poll(poll_result_t *res_poll);
static int proc_res_poll_read(int *arr, int size);
static void proc_res_poll_err(int *arr, int size);

/*
 * receiving -- routine for receiving messages and adding them to the processing queue
 * @arg -- tid of main thread
 */
void* receiving(void* arg)
{
	poll_result_t *res_poll = NULL;
	pthread_t tid_main = *((pthread_t*)arg);

	while (1)
	{
		res_poll = NULL;

		if (poll_handler(&res_poll) == -1) goto failure;

		if (proc_res_poll(res_poll) == -1) goto failure;

		log_info("Receiving success...", NULL);
	}

failure:
	log_info("Receiving failed...", NULL);
	pthread_kill(tid_main, SIGINT);
	pthread_exit(NULL);
}

//static_functions_________________________________
/*
 * proc_res_poll -- processing @res_poll;
 * @res_poll -- result of poll_handler();
 *
 * return:
 * 0 -- success;
 * -1 -- fail;
 */
int proc_res_poll(poll_result_t *res_poll)
{
	if (res_poll != NULL)
	{
		if (res_poll->set_err != NULL)
		{
			proc_res_poll_err(res_poll->set_err, res_poll->counter[1]);
		}

		if (res_poll->set_read != NULL)
		{
			if (proc_res_poll_read(res_poll->set_read, res_poll->counter[0]) == -1)
			{
				log_debug("FAIL: proc_res_poll_read:", NULL);
				goto failure;
			}
		}

		free_poll_result(res_poll);
	}

	return 0;
failure:
	return -1;
}

/*
 * proc_res_poll_read -- processing of all sockets where POLLIN occurred
 * @arr -- array of sockets;
 * @size -- size of array;
 *
 * return:
 * 0 -- success;
 * -1 -- fail;
 */
int proc_res_poll_read(int *arr, int size)
{
	int i = 0, fd = 0;
	message_t *msg = NULL;

	for (i = 0; i < size; ++i)
	{
		if (arr[i] == listen_sfd)
		{
			if ((fd = accept_without_addr(listen_sfd)) == -1)
			{
				log_debug("FAIL: accept_without_addr: ", NULL);
				goto failure;
			}

			add_pollfd(fd, POLLIN);
		}
		else if ((msg = recv_message(arr[i])) == NULL)
		{
			del_pollfd(arr[i]);
			insert_to_que_msg(QUE_DEL, arr[i], NULL);
		}
		else
		{
			insert_to_que_msg(QUE_NORM, arr[i], msg);
		}
	}

	return 0;
failure:
	return -1;
}

/*
 * proc_res_poll_err -- processing of all sockets where
 * any events except POLLIN occurred
 * @arr -- array of sockets;
 * @size -- size of array;
 *
 * return:
 * 0 -- success;
 * -1 -- fail;
 */
void proc_res_poll_err(int *arr, int size)
{
	int i = 0;

	for (i = 0; i < size; ++i)
	{
		del_pollfd(arr[i]);
		insert_to_que_msg(QUE_DEL, arr[i], NULL);
	}
}

//_________________________________________________
