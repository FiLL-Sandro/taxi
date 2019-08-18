#include "list.h"

/*
 * insert_free_item -- create and insert a record in the desired queue
 * @tailhead -- queue head;
 * @sfd -- value of item;
 *
 * retun:
 * NULL -- failure;
 * non-NULL -- pointer to created item;
 */
free_item_t* insert_free_item(struct tailhead *tailhead, u_int sfd)
{
	free_item_t *tmp_item = (free_item_t*)malloc(sizeof(free_item_t));

	if (tmp_item == NULL)
	{
		log_err_with_perror("FAIL: malloc: memory for new free_item_t", NULL);
		goto ret_f;
	}

	tmp_item->sfd = sfd;
	TAILQ_INSERT_TAIL(tailhead, tmp_item, navi);

ret_f:
	return tmp_item;
}

/*
 * remove_free_item -- remove item from queue
 * @tailhead -- queue head;
 * @elm -- target item;
 */
void remove_free_item(struct tailhead *tailhead, free_item_t *elm)
{
	TAILQ_REMOVE(tailhead, elm, navi);
	free(elm);
}
