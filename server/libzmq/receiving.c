#include "receiving.h"

static int proc_res_poll(poll_result_t *res_poll);
static void proc_res_poll_read(void);
static int proc_res_poll_err(int *arr, int size);

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
			if (proc_res_poll_err(res_poll->set_err, res_poll->counter[1]) == -1)
			{
				log_debug("FAIL: proc_res_poll_err: error with server_fd", NULL);
				goto failure;
			}
		}

		free_poll_result(res_poll);
	}

	proc_res_poll_read();

	return 0;
failure:
	return -1;
}

/*
 * proc_res_poll_read -- processing of all sockets where POLLIN occurred
 */
void proc_res_poll_read()
{
	int fd = 0;
	message_t *msg = NULL;
	zmq_msg_t zmq_msg;
	que_msg_t *que_msg = NULL;

	zmq_msg_init(&zmq_msg);
	if ((msg = recv_message(&zmq_msg, serv_sock, ZMQ_DONTWAIT)) == NULL)
	{
		log_debug("WARNING: recv_message: no messages or error occured");
		goto ret_f;
	}

	if ((fd = zmq_msg_get(&zmq_msg, ZMQ_SRCFD)) == -1)
	{
		log_debug("FAIL: zmq_msg_get: %s", zmq_strerror(errno));
		goto ret_f;
	}
	else if (msg->type == MSG_AUTH_DRI || msg->type == MSG_AUTH_PASS)
	{
		add_pollfd(fd, 0);
		que_msg = crt_set_que_msg(QUE_NORM, fd, &zmq_msg, msg);
	}
	else
	{
		que_msg = crt_set_que_msg(QUE_NORM, fd, NULL, msg);
	}

	insert_to_que_msg(que_msg);

ret_f:
	return;
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
int proc_res_poll_err(int *arr, int size)
{
	int i = 0;
	que_msg_t *que_msg = NULL;

	for (i = 0; i < size; ++i)
	{
		if (arr[i] == server_fd)
		{
			goto failure;
		}

		del_pollfd(arr[i]);
		que_msg = crt_set_que_msg(QUE_DEL, arr[i], NULL, NULL);
		insert_to_que_msg(que_msg);
	}

	return 0;
failure:
	return -1;
}

//_________________________________________________
