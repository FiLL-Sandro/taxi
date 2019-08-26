#ifndef POLL_OPER_H
#define POLL_OPER_H

#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <stdbool.h>
#include "logging.h"

#define SETS 2

typedef struct poll_result
{
	int *set_read;
	int *set_err;
	int counter[SETS];
} poll_result_t;

extern int add_pollfd(int fd, short events);
extern int del_pollfd(int fd);
extern int poll_handler(poll_result_t** result);
extern void free_poll_result(poll_result_t *result);

#endif
