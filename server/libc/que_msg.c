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
 * insert_to_que_msg -- insert message into que_msg
 * @type -- message type;
 * @sfd -- target sfd;
 * @data -- message data;
 */
void insert_to_que_msg(que_msg_type_t type, int sfd, void *data)
{
	que_msg_t *que_msg = (que_msg_t*)malloc(sizeof(que_msg_t));

	que_msg->sfd = sfd;
	que_msg->data = data;
	que_msg->type = type;

	pthread_mutex_lock(&mutex);

	switch (type)
	{
		case QUE_NORM: case QUE_DEL:
			TAILQ_INSERT_TAIL(&que_msg_head, que_msg, navi);
			break;
	}

	pthread_mutex_unlock(&mutex);
}
