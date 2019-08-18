#include "receiving.h"

static int proc_res_poll(poll_result_t *res_poll);
static int proc_res_poll_read(int *arr, int size);
static void proc_res_poll_err(int *arr, int size);
static void insert_que_msg(que_msg_type_t type, int sfd, void *data);
static que_msg_t* check_que_msg(void);

/*
 * receiving -- receiving messages and adding them to the processing queue
 *
 * return:
 * 0 -- success;
 * -1 -- fail;
 */
int receiving(void)
{
	poll_result_t *res_poll = NULL;
	que_msg_t *que_msg = NULL;

	while ((que_msg = check_que_msg()) != NULL)
	{
		close(que_msg->sfd);
		del_pollfd(que_msg->sfd);
		free(que_msg), que_msg = NULL;
	}

	if (poll_handler(&res_poll) == -1) goto failure;

	if (proc_res_poll(res_poll) == -1) goto failure;

	log_info("Receiving success...", NULL);
	return 0;
failure:
	log_info("Receiving failed...", NULL);
	return -1;
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
	bool flag = false;

	if (res_poll != NULL)
	{
		if (res_poll->set_err != NULL)
		{
			proc_res_poll_err(res_poll->set_err, res_poll->counter[1]);
		}

		if (res_poll->set_read != NULL)
		{
			flag = proc_res_poll_read(res_poll->set_read, res_poll->counter[0]) == -1
					? true
					: false;
		}

		free_poll_result(res_poll);
	}

	return flag == false ? 0 : -1;
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
	que_msg_t *que_msg = NULL;

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
			close(arr[i]);
			del_pollfd(arr[i]);
			insert_que_msg(QUE_DEL, arr[i], NULL);
		}
		else
		{
			insert_que_msg(QUE_NORM, arr[i], msg);
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
		close(arr[i]);
		del_pollfd(arr[i]);
		insert_que_msg(QUE_DEL, arr[i], NULL);
	}
}

/*
 * insert_que_msg -- insert message into @que_msg
 * @type -- message type;
 * @sfd -- target sfd;
 * @data -- message data;
 */
void insert_que_msg(que_msg_type_t type, int sfd, void *data)
{
	que_msg_t *que_msg = (que_msg_t*)malloc(sizeof(que_msg_t));

	que_msg->type = type;
	que_msg->sfd = sfd;
	que_msg->data = data;
	if (data == NULL) TAILQ_INSERT_HEAD(&que_msg_head, que_msg, navi);
				 else TAILQ_INSERT_TAIL(&que_msg_head, que_msg, navi);
}

/*
 * check_que_msg -- check message queue for message desire type
 *
 * return:
 * NULL -- no messages;
 * non-NULL -- pointer to message;
 */
que_msg_t* check_que_msg(void)
{
	que_msg_t *que_msg = NULL;

	if (!TAILQ_EMPTY(&que_msg_head))
	{
		que_msg = TAILQ_FIRST(&que_msg_head);

		if (que_msg->type == QUE_POLL_DEL)
		{
			TAILQ_REMOVE(&que_msg_head, que_msg, navi);
		}
		else
		{
			que_msg = NULL;
		}
	}

	return  que_msg;
}

//_________________________________________________
