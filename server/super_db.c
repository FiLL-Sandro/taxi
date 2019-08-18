#include "super_db.h"

typedef struct entry
{
	kind_host_t kind;
	int dest_fd;
	free_item_t *p_free;
	void *data;
} entry_t;

static vec_i_t *db = NULL;
static u_int size_db = 0;
static struct tailhead
	free_dri_head = TAILQ_HEAD_INITIALIZER(free_dri_head),
	free_pass_head = TAILQ_HEAD_INITIALIZER(free_pass_head);


static struct tailhead* take_free_head(kind_host_t kind);
static int* take_driver_pos(u_int sfd);
static int* take_passenger_route(u_int sfd);
static free_item_t* find_driver(free_item_t *tmp_pass);
static void prepare_peers_for_session(free_item_t *dri, free_item_t *pass);
static int set_entry(entry_t *ent, int sfd, kind_host_t kind, int dest_fd, void *data);
static void unset_entry(entry_t *ent, kind_host_t kind);

/*
 * add_entry -- add record in db;
 * @sfd -- socket/cell;
 * @kind -- kind of peer;
 * @data -- data of peer;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int add_entry(int sfd, kind_host_t kind, void *data)
{
	entry_t *tmp_ent = (entry_t*)malloc(sizeof(entry_t));

	if (tmp_ent == NULL || sfd < 0 || data == NULL)
	{
		log_debug("FAIL: initialization error;\n"
				  "(tmp_ent: %p, sfd: %d, data: %p)",
				  (void*)tmp_ent, sfd, (void*)data);
		goto failure;
	}
	else if (set_entry(tmp_ent, sfd, kind, -1, data) == -1)
	{
		log_debug("FAIL: entry setting are failed", NULL);
		goto failure_after_set;
	}
	else if (add_item_vec(&db, &size_db, tmp_ent, (u_int)sfd) == -1)
	{
		log_debug("FAIL: add_item: adding into db are failed", NULL);
		goto failure_after_set;
	}

	goto success;

failure_after_set:
	unset_entry(tmp_ent, kind);
failure:
	if (tmp_ent != NULL) free(tmp_ent);
	return -1;
success:
	log_info("Entry was added;(kind: %u)", kind);
	return 0;
}

/*
 * del_entry -- delete record from db
 * @sfd -- cell;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int del_entry(int sfd)
{
	entry_t *tmp_ent = (entry_t*)take_data_vec(db, size_db, (u_int)sfd);

	if (tmp_ent == NULL || sfd < 0)
	{
		log_debug("FAIL: initialization error;\n"
				  "(tmp_ent: %p, sfd: %d)",
				  (void*)tmp_ent, sfd);
		goto failure;
	}

	unset_entry(tmp_ent, tmp_ent->kind);
	if (del_item_vec(&db, &size_db, (u_int)sfd) == -1)
	{
		log_debug("FAIL: del_item_vec: deleting from vector are failed", NULL);
		goto failure;
	}

	log_info("Entry was deleted...", NULL);
	return 0;
failure:
	return -1;
}

/*
 * set_session -- creating a session between the passenger and the driver
 * @dri_fd -- value-result which storage driver socket;
 *
 * return:
 * NULL -- session cannot be created;
 * non-NULL -- session is created;
 */
passenger_t* set_session(int *dri_fd)
{
	free_item_t *tmp_dri = NULL, *tmp_pass = NULL;
	passenger_t *pass_data = NULL;
	entry_t *tmp_ent = NULL;

	if (dri_fd == NULL)
	{
		log_debug("FAIL: no place where located result\n"
				  "(dri_fd: %p)", (void*)dri_fd);
	}
	else if (!TAILQ_EMPTY(&free_pass_head) && !TAILQ_EMPTY(&free_dri_head))
	{
		tmp_pass = TAILQ_FIRST(&free_pass_head);
		tmp_dri = find_driver(tmp_pass);

		tmp_ent = take_data_vec(db, size_db, tmp_pass->sfd);
		pass_data = (passenger_t*)tmp_ent->data;

		*dri_fd = (int)tmp_dri->sfd;
		prepare_peers_for_session(tmp_dri, tmp_pass);

		log_info("Session are set...", NULL);
	}

	return pass_data;
}

/*
 * change_position -- change current peer position
 * @sfd -- db cell;
 * @pos -- new value;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int change_position(int sfd, int *pos)
{
	entry_t *tmp_ent = take_data_vec(db, size_db, (u_int)sfd);

	if (tmp_ent == NULL || pos == NULL || sfd < 0)
	{
		log_debug("FAIL: entry taking are failed or pos is missing;\n"
				  "(tmp_ent: %p, pos: %p, sfd: %d)",
				  (void*)tmp_ent, (void*)pos, sfd);
		goto failure;
	}

	switch (tmp_ent->kind)
	{
		case HOST_DRIVER:
			((driver_t*)tmp_ent->data)->coord[0] = pos[0];
			((driver_t*)tmp_ent->data)->coord[1] = pos[1];
			break;
		case HOST_PASSENGER:
			((passenger_t*)tmp_ent->data)->route[0] = pos[0];
			((passenger_t*)tmp_ent->data)->route[1] = pos[1];
			break;
		default:
			log_debug("FAIL: unknown kind;(kind: %u)", tmp_ent->kind);
			goto failure;
			break;
	}

	log_info("Position was changed...", NULL);
	return 0;
failure:
	return -1;
}

/*
 * change_status -- change @stat field into entry
 * @sfd -- db cell;
 * @kind -- kind of peer;
 * @stat -- new value;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int change_status(int sfd, kind_host_t kind, void *stat)
{
	entry_t *tmp_ent = take_data_vec(db, size_db, (u_int)sfd);

	if (sfd < 0 || stat == NULL || tmp_ent == NULL)
	{
		log_debug("FAIL: sfd is negative, no data for chenging or tmp_ent isn't exist;\n"
				  "(sfd: %d, stat: %p, tmp_ent: %p)",
				  sfd, (void*)stat, (void*)tmp_ent);
		goto failure;
	}
	else if (tmp_ent->kind != kind)
	{
		log_debug("FAIL: kinds isn't coinside;\n"
				  "(tmp_ent.kind: %u, kind: %u)",
				  tmp_ent->kind, kind);
		goto failure;
	}

	switch (kind)
	{
		case HOST_DRIVER:
			((driver_t*)tmp_ent->data)->stat = *((d_stat_t*)stat);
			break;
		case HOST_PASSENGER:
			((passenger_t*)tmp_ent->data)->stat = *((p_stat_t*)stat);
			break;
		default:
			log_debug("FAIL: unknown kind;(kind: %u)", kind);
			goto failure;
			break;
	}

	log_info("Status was changed...", NULL);
	return 0;
failure:
	return -1;
}

/*
 * set_free_peer -- mark peer as free
 * @sfd -- db cell;
 * @pos -- new position value;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int set_free_peer(int sfd, int *pos)
{
	entry_t *tmp_ent = take_data_vec(db, size_db, (u_int)sfd);
	d_stat_t d_stat = DRI_FREE;
	p_stat_t p_stat = PASS_WAIT;

	if (sfd < 0 || tmp_ent == NULL)
	{
		log_debug("FAIL: initialization error;\n"
				  "(sfd: %d, tmp_ent: %p",
				  sfd, (void*)tmp_ent);
		goto failure;
	}
	else if (set_entry(tmp_ent, sfd, tmp_ent->kind, -1, tmp_ent->data) == -1)
	{
		log_debug("FAIL: entry setting are failed", NULL);
		goto failure;
	}

	if (pos != NULL) change_position(sfd, pos);
	tmp_ent->kind == HOST_DRIVER ? change_status(sfd, tmp_ent->kind, &d_stat)
								 : change_status(sfd, tmp_ent->kind, &p_stat);

	log_info("Peer freed...", NULL);
	return 0;
failure:
	return -1;
}

int take_dest_fd(int sfd)
{
	entry_t *tmp_ent = take_data_vec(db, size_db, (u_int)sfd);

	if (sfd < 0 || tmp_ent == NULL)
	{
		log_debug("FAIL: initialization error;\n"
				  "(sfd: %d, tmp_ent: %p",
				  sfd, (void*)tmp_ent);
		goto failure;
	}

	log_info("Destination are taken...", NULL);
	return tmp_ent->dest_fd;
failure:
	return -1;
}

//static_function______________________________________

/*
 * set_entry -- entry initialization
 * @ent -- pointer to entry;
 * @sfd -- db cell;
 * @kind -- kind of peer;
 * @dest_fd -- fd destination;
 * @data -- peer data;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int set_entry(entry_t *ent, int sfd, kind_host_t kind, int dest_fd, void *data)
{
	struct tailhead *tailhead = take_free_head(kind);

	if (tailhead == NULL)
	{
		log_debug("FAIL: unknown kind;(kind: %u)", kind);
		goto failure;
	}

	ent->kind = kind;
	ent->data = data;
	ent->dest_fd = dest_fd;
	ent->p_free = insert_free_item(tailhead, (u_int)sfd);

	if (ent->p_free == NULL)
	{
		log_debug("FAIL: insert_free_item: troubles with adding to free_queue", NULL);
		goto failure;
	}

	return 0;
failure:
	return -1;
}

/*
 * unset_entry -- deinitialization of a record
 * @ent -- pointer to entry;
 * @kind -- kind of peer;
 *
 */
void unset_entry(entry_t *ent, kind_host_t kind)
{
	struct tailhead *tailhead = take_free_head(kind);

	if (ent->data != NULL)
	{
		free(ent->data);
	}
	if (ent->p_free != NULL)
	{
		remove_free_item(tailhead, ent->p_free);
	}

	ent->data = ent->p_free = NULL;
	ent->dest_fd = -1;
}

/*
 * take_driver_pos -- take the value of the @pos field from the driver
 * @sfd -- cell;
 *
 * return:
 * non-NULL -- value of @pos;
 */
int* take_driver_pos(u_int sfd)
{
	entry_t *tmp_dri = take_data_vec(db, size_db, sfd);

	return ((driver_t*)tmp_dri->data)->coord;
}

/*
 * take_passenger_route -- take the value of the @route field from the passenger
 * @sfd -- cell;
 *
 * return:
 * value of @route;
 */
int* take_passenger_route(u_int sfd)
{
	entry_t *tmp_pass = take_data_vec(db, size_db, sfd);

	return ((passenger_t*)tmp_pass->data)->route;
}

/*
 * take_free_head -- choosing the right queue with free peers
 * @kind -- kind of peer;
 *
 * return:
 * non-NULL -- pointer to queue head;
 */
struct tailhead* take_free_head(kind_host_t kind)
{
	struct tailhead *tailhead = NULL;

	switch (kind)
	{
		case HOST_DRIVER: tailhead = &free_dri_head; break;
		case HOST_PASSENGER: tailhead = &free_pass_head; break;
		default:
			log_debug("FAIL: kind unknown;(kind: %u)", kind);
			tailhead = NULL;
	}

	return tailhead;
}

/*
 * find_driver -- driver search for a passenger
 * @tmp_pass -- target passenger;
 *
 * return:
 * non-NULL -- pointer to free driver;
 */
free_item_t* find_driver(free_item_t *tmp_pass)
{
	free_item_t *tmp_item = NULL, *tmp_dri = NULL;
	int min = -1, *pos = NULL, i = 0, *route = NULL;

	route = take_passenger_route(tmp_pass->sfd);
	TAILQ_FOREACH(tmp_item, &free_dri_head, navi)
	{
		pos = take_driver_pos(tmp_item->sfd);

		i = round(sqrt(pow((double)(pos[0] - route[0]), 2.) +
				pow((double)(pos[1] - route[1]), 2.)));

		if (min == -1 || min > i)
		{
			tmp_dri = tmp_item, min = i;
		}
	}

	return  tmp_dri;
}

/*
 * prepare_peers_for_session -- preparing peers for interaction in the session
 * @dri -- driver;
 * @pass -- passenger;
 */
void prepare_peers_for_session(free_item_t *dri, free_item_t *pass)
{
	entry_t *dri_ent = take_data_vec(db, size_db, dri->sfd);
	entry_t *pass_ent = take_data_vec(db, size_db, pass->sfd);

	dri_ent->dest_fd = (int)pass->sfd, pass_ent->dest_fd = (int)dri->sfd;
	remove_free_item(&free_dri_head, dri), dri_ent->p_free = NULL;
	remove_free_item(&free_pass_head, pass), pass_ent->p_free = NULL;
}
//_____________________________________________________
