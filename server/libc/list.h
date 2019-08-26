#ifndef LIST_H
#define LIST_H

#include "common_data.h"
#include "logging.h"
#include <stdlib.h>
#include <sys/queue.h>

typedef struct free_item
{
	TAILQ_ENTRY(free_item) navi;
	u_int sfd;
} free_item_t;

TAILQ_HEAD(tailhead, free_item);

extern free_item_t* insert_free_item(struct tailhead *tailhead, u_int sfd);
extern void remove_free_item(struct tailhead *tailhead, free_item_t *elm);
#endif
