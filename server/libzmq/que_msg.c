#include "que_msg.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * take_que_msg -- to take message from message queue;
 * @mask -- type set;
 *
 * return:
 * non-NULL -- valid message;
 * NULL -- invalid message;
 */
que_msg_t* take_que_msg(que_msg_type_t mask)
{
	que_msg_t *que_msg = NULL;

	pthread_mutex_lock(&mutex);

	if (!TAILQ_EMPTY(&que_msg_head))
	{
		que_msg = TAILQ_FIRST(&que_msg_head);
		if ((mask & que_msg->type) != 0)
		{
			TAILQ_REMOVE(&que_msg_head, que_msg, navi);
		}
		else
		{
			que_msg = NULL;
		}
	}

	pthread_mutex_unlock(&mutex);

	return  que_msg;
}

/*
 * insert_to_que_msg -- insert message into message queue
 * @que_msg -- inserting message;
 */
void insert_to_que_msg(que_msg_t *que_msg)
{
	pthread_mutex_lock(&mutex);

	switch (que_msg->type)
	{
		case QUE_NORM: case QUE_DEL:
			TAILQ_INSERT_TAIL(&que_msg_head, que_msg, navi);
			break;
	}

	pthread_mutex_unlock(&mutex);
}

/*
 * crt_set_que_msg -- create and setting que_msg_t item;
 * @type -- que_msg type;
 * @sfd -- source fd;
 * @zmq_msg -- zmq_msg_t for communication with sender;
 * @data -- data received from sender;
 *
 * return:
 * non-NULL -- success: pointer to valid que_msg_t;
 * NULL -- failure;
 */
que_msg_t* crt_set_que_msg(que_msg_type_t type, int sfd, zmq_msg_t *zmq_msg, void *data)
{
	que_msg_t *que_msg = (que_msg_t*)malloc(sizeof(que_msg_t));

	que_msg->sfd = sfd;
	que_msg->data = data;
	que_msg->type = type;
	if (zmq_msg != NULL) que_msg->zmq_msg = *zmq_msg;

	return que_msg;
}

/*
 * free_que_msg -- freed to que_msg_t
 * @que_msg -- pointer to que_msg_t;
 */
void free_que_msg(que_msg_t *que_msg)
{
	if (que_msg != NULL)
	{
		if (que_msg->data != NULL)
			free(que_msg->data), que_msg->data = NULL;

		free(que_msg);
	}
}
