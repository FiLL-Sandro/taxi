#include "driver.h"

kind_host_t my_host = HOST_DRIVER;
struct sockaddr_in *serv_addr = NULL;
int sfd = 0;
driver_t *pri_data = NULL;
int *route = NULL;
static d_stat_t resp_pass_wait[]	= {DRI_RIDE_TO, DRI_ON_SIDE, DRI_IGNOR};
static d_stat_t resp_pass_load[]	= {DRI_IGNOR};
static d_stat_t resp_pass_ride_to[] = {DRI_IMP_ORD, DRI_COMP, DRI_IGNOR};
d_stat_t *resp_arr[RESP_STAT] =
{
	resp_pass_wait,		// PASS_WAIT
	resp_pass_load,		// PASS_LOAD
	resp_pass_ride_to,	// PASS_RIDE_TO
	NULL				// PASS_COMP
};

int main(int argc, char *argv[])
{
	if (init_driver(argc, argv) == -1) goto failure;
	log_info("Driver was initialized and registrated by server...", NULL);

	while (1)
	{
		if (recv_proc() == -1) goto failure;
	}

	close(sfd);
	free(serv_addr);
	log_info("Driver success...", NULL);
	exit(EXIT_SUCCESS);
failure:
	close(sfd);
	free(serv_addr);
	log_info("Driver fail...", NULL);
	exit(EXIT_FAILURE);
}
