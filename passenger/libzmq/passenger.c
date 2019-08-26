#include "passenger.h"

void *pass_sock = NULL, *pass_ctx = NULL;
int passenger_fd = -1;
bool flag_complete_work = false;
kind_host_t my_host = HOST_PASSENGER;
passenger_t *pri_data = NULL;
zmq_msg_t pass_zmq_msg;
struct pollfd pass_poll;
static p_stat_t resp_dri_ride_to[] = {PASS_IGNOR};
static p_stat_t resp_dri_on_side[] = {PASS_LOAD, PASS_RIDE_TO, PASS_IGNOR};
static p_stat_t resp_dri_imp_ord[] = {PASS_IGNOR};
static p_stat_t resp_dri_comp[] = {PASS_COMP, PASS_IGNOR};
p_stat_t *resp_arr[RESP_STAT] =
{
	NULL,
	resp_dri_ride_to,
	resp_dri_on_side,
	resp_dri_imp_ord,
	resp_dri_comp
};

int main(int argc, char *argv[])
{
	if (init_pass(argc, argv) == -1) goto failure;
	log_info("Passenger initialization...", NULL);

	while (flag_complete_work == false)
	{
		if (recv_proc() == -1) goto failure;
	}

	log_info("Passenger success", NULL);
	exit(EXIT_SUCCESS);
failure:
	log_debug("PASSENGER CRASHED", NULL);
	exit(EXIT_FAILURE);
}

