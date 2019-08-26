#include "recv_proc.h"

#define TIME_WAIT 5
#define POLL_TIMEOUT 500

static int proc_msg_init_sess(message_t *msg);
static int proc_msg_trans(void* data);
static int riding(int x1, int y1, int x2, int y2);

static bool in_session = false;


/*
 * recv_proc -- receiving and processing messages
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int recv_proc(void)
{
	message_t *msg = NULL;
	p_stat_t p_stat;
	int amount = poll(&dri_fd_poll, 1, POLL_TIMEOUT);

	log_info("fd: %d, revents_dri: %d", dri_fd_poll.fd, dri_fd_poll.revents);
	if (amount != 0)
	{
		log_err_with_perror("FAIL: poll: connection interrupted or error with poll()", NULL);
		goto failure;
	}

	if ((msg = recv_message(&dri_zmq_msg, dri_sock, ZMQ_DONTWAIT)) == NULL)
	{
		log_debug("FAIL: recv_message: receiving message are failed", NULL);
		goto success;
	}

	if (in_session == true) alarm(0);
	switch (msg->type)
	{
		case MSG_INIT_SESSION:
			if (proc_msg_init_sess(msg) == -1) goto failure;
			p_stat = PASS_WAIT;
			if (proc_msg_trans(&p_stat) == -1) goto failure;
			break;
		case MSG_TRANSIT_FROM_PASS:
			if (proc_msg_trans(msg->data) == -1) goto failure;
			break;
		default:
			log_debug("FAIL: bad message type;(type: %u)", msg->type);
			goto failure;
	}
	if (in_session == true) alarm(TIME_WAIT);

success:
	log_info("Driver now: %u", pri_data->stat);
	if (msg != NULL) free(msg), msg = NULL;
	return 0;
failure:
	if (msg != NULL) free(msg), msg = NULL;
	return -1;
}

//static_functions_____________________________________________
/*
 * proc_msg_init_sess -- initialization message processing
 * @msg -- pointer to message;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int proc_msg_init_sess(message_t *msg)
{
	passenger_t *pass = (passenger_t*)msg->data;

	if (route != NULL)
	{
		log_debug("FAIL: reinitialization", NULL);
		goto failure;
	}

	in_session = true;
	route = (int*)malloc(sizeof(int) * 4);
	memcpy(route, pass->route, sizeof(int) * 4);

	if (riding(pri_data->coord[0], pri_data->coord[1],
			   route[0], route[1]) == -1)
	{
		log_debug("FAIL: riding:", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}

int riding(int x1, int y1, int x2, int y2)
{
	int S = 0, t = 0, i = 0, lambda = 0;

	S = round(sqrt(pow((double)(x1 - x2), 2.) +
			pow((double)(y1 - y2), 2.)));

	t = S / pri_data->speed;

	for (i = 0; i < t; ++i)
	{
		sleep(1);
		lambda = i / (t - i);
		pri_data->coord[0] = (x1 + x2 * lambda) / (1 + lambda);
		pri_data->coord[1] = (y1 + y2 * lambda) / (1 + lambda);

		if (send_message(&dri_zmq_msg, dri_sock, MSG_DRI_CUR_POS,
						 pri_data->coord, sizeof(int) * 2) == -1)
		{
			log_debug("FAIL: send_message: message with current position", NULL);
			goto failure;
		}
		wrap_recv(&dri_zmq_msg, dri_sock, 0);

		log_info("cur_pos: %d,%d; target_pos: %d,%d",
				 pri_data->coord[0], pri_data->coord[1], x2, y2);
	}

	return 0;
failure:
	return -1;
}

/*
 * proc_msg_trans -- processing all messages and sending a response
 * @data -- message data;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int proc_msg_trans(void *data)
{
	type_msg_t type_msg;
	size_t size_data = 0;
	p_stat_t p_stat = *((p_stat_t*)data);
	d_stat_t *tmp_resp = NULL;
	int i = 0;
	void *data_for_resp = NULL;

	if (p_stat > RESP_STAT)
	{
		log_debug("FAIL: p_stat has bad value;(p_stat: %u)", p_stat);
		goto failure;
	}
	else if ((tmp_resp = resp_arr[p_stat]) == NULL)
	{
		log_debug("FAIL: peer cannot receiving and processing this @p_stat;(p_stat: %u)", p_stat);
		goto failure;
	}

	for (i = 0; tmp_resp[i] != DRI_IGNOR; ++i)
	{
		switch (tmp_resp[i])
		{
			case DRI_RIDE_TO: case DRI_ON_SIDE: case DRI_IMP_ORD:
				type_msg = MSG_TRANSIT_FROM_DRI;
				size_of_message_data(type_msg, &size_data);
				data_for_resp = malloc(size_data);

				*((d_stat_t*)data_for_resp) = pri_data->stat = resp_arr[p_stat][i];
				break;
			case DRI_COMP:
				if (riding(route[0], route[1],
						   route[2], route[3]) == -1)
				{
					log_debug("FAIL: riding:", NULL);
					goto failure;
				}

				type_msg = MSG_ORDER_COMP_DRI;
				size_of_message_data(type_msg, &size_data);
				data_for_resp = malloc(size_data);

				((int*)data_for_resp)[0] = pri_data->coord[0] = route[2];
				((int*)data_for_resp)[1] = pri_data->coord[1] = route[3];
				pri_data->stat = DRI_FREE;
				free(route), route = NULL;
				in_session = false, alarm(0);
				break;
		}

		if (send_message(&dri_zmq_msg, dri_sock, type_msg, data_for_resp, size_data) == -1)
		{
			log_debug("FAIL: send_message: message sending to server are failed", NULL);
			goto failure;
		}

		if(data_for_resp != NULL) free(data_for_resp), data_for_resp = NULL;
	}

	return 0;
failure:
	if (data_for_resp != NULL) free(data_for_resp), data_for_resp = NULL;
	return -1;
}
//_____________________________________________________________
