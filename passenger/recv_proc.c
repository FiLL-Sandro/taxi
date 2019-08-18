#include "recv_proc.h"

static int proc_ord_comp_dri(void);
static int proc_trans_msg(void *data);

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

	if ((msg = recv_message(sfd)) == NULL)
	{
		log_debug("FAIL: recv_message: ", NULL);
		goto failure;
	}

	switch (msg->type)
	{
		case MSG_ORDER_COMP_DRI:
			proc_ord_comp_dri();
			break;
		case MSG_TRANSIT_FROM_DRI:
			if (proc_trans_msg(msg->data) == -1) goto failure;
			break;
		default:
			log_debug("FAIL: bad message type;(type: %u)", msg->type);
			goto failure;
	}

	log_info("Passenger: %u", pri_data->stat);
	if (msg != NULL) free(msg), msg = NULL;
	return 0;
failure:
	if (msg != NULL) free(msg), msg = NULL;
	return -1;
}
//static_functions____________________________________________
/*
 * proc_ord_comp_dri -- processing session completion message
 *
 * return:
 * 0 -- success;
 */
int proc_ord_comp_dri(void)
{
	void *data = NULL;
	size_t size_data;
	type_msg_t type_msg = MSG_ORDER_COMP_PASS;

	size_of_message_data(type_msg, &size_data);
	send_message(sfd, type_msg, data, size_data);

	flag_complete_work = true;
	return 0;
}

/*
 * proc_trans_msg -- trasit message processing
 * @data -- message data
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int proc_trans_msg(void *data)
{
	size_t size_data = 0;
	type_msg_t type_msg;
	p_stat_t *tmp_resp = NULL;
	d_stat_t d_stat = *((d_stat_t*)data);
	int j = 0;

	if (d_stat > RESP_STAT)
	{
		log_debug("FAIL: bad data received;(d_stat: %u)", d_stat);
		goto failure;
	}
	else if ((tmp_resp = resp_arr[d_stat]) == NULL)
	{
		log_debug("FAIL: peer cannot response and received given @d_stat\n"
				  "(d_stat: %u)", d_stat);
		goto failure;
	}

	for (j = 0; tmp_resp[j] != PASS_IGNOR; ++j)
	{
		switch (tmp_resp[j])
		{
			case PASS_LOAD: case PASS_RIDE_TO:
				type_msg = MSG_TRANSIT_FROM_PASS;
				size_of_message_data(type_msg, &size_data);
				data = malloc(size_data);

				*((p_stat_t*)data) = pri_data->stat = tmp_resp[j];
				break;
		}

		if (send_message(sfd, type_msg, data, size_data) == -1)
		{
			log_debug("FAIL: send_message: message about close session", NULL);
			if (data != NULL) free(data), data = NULL;
			goto failure;
		}

		if (data != NULL) free(data), data = NULL;
	}

	return 0;
failure:
	return -1;
}
//____________________________________________________________
