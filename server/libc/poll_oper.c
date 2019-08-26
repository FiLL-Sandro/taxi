#include "poll_oper.h"

#define TIMEOUT_POLL 500
#define RESERV_ITEM 5

static struct pollfd *arr_poll = NULL;
static nfds_t size_arr_poll = 0;


typedef enum set_id_e
{
	SET_READ = 0,
	SET_ERR
} set_id_t;

static struct pollfd* realloc_for_poll(nfds_t new_size_arr_poll);
static int compare_int_and_pollfd(const void* x, const void* y);
static int compare_pollfd(const void* x, const void* y);
static int cleaning_arr_poll(void);
static bool add_to_set(poll_result_t *result, set_id_t id, int fd);

/*
 * compare_int_and_pollfd -- comparator of pair of int and pollfd
 * @x -- pointer to int;
 * @y -- pointer to pollfd;
 */
int compare_int_and_pollfd(const void* x, const void* y)
{
	int x1 = (*(const int*)x) < 0 ? -(*(const int*)x) : (*(const int*)x);
	int y1 = ((const struct pollfd*)y)->fd < 0 ? -((const struct pollfd*)y)->fd : ((const struct pollfd*)y)->fd;

	return x1 - y1;
}

/*
 * compare_pollfd -- comparator of pair of pollfd and pollfd
 * @x -- pointer to pollfd;
 * @y -- pointer to pollfd;
 */
int compare_pollfd(const void* x, const void* y)
{
	int x1 = ((const struct pollfd*)x)->fd < 0 ? -((const struct pollfd*)x)->fd : ((const struct pollfd*)x)->fd;
	int y1 = ((const struct pollfd*)y)->fd < 0 ? -((const struct pollfd*)y)->fd : ((const struct pollfd*)y)->fd;

	return x1 - y1;
}

/*
 * realloc_for_poll -- @arr_poll reallocation and change of him @size_arr_poll
 * @new_size_arr_poll -- new value of @size_arr_poll;
 *
 * return:
 * NULL -- failure or @arr_poll == NULL;
 * non-NULL -- pointer to new area for @arr_poll;
 */
struct pollfd* realloc_for_poll(nfds_t new_size_arr_poll)
{
	struct pollfd *new_arr_poll = NULL;

	if (new_size_arr_poll == 0)
	{
		free(arr_poll), arr_poll = NULL;
		size_arr_poll = new_size_arr_poll;
		goto ret_f;
	}

	new_arr_poll = (struct pollfd*)realloc(arr_poll, new_size_arr_poll * sizeof(struct pollfd));
	if (new_arr_poll != NULL)
	{
		arr_poll = new_arr_poll, size_arr_poll = new_size_arr_poll;
	}

ret_f:
	return new_arr_poll;
}

/*
 * add_pollfd -- adding @fd into @arr_poll
 * @fd -- target fd;
 * @events -- events for listening;
 *
 * return:
 * >=0 -- success: new pollfd index;
 * -1 -- failure;
 */
int add_pollfd(int fd, short int events)
{
	int ret_f = 0;
	struct pollfd *tmp_buf = NULL;

	if (events < 0)
	{
		log_debug("WARNING: events < 0;(events: %d)", events);
		ret_f = -1;
		goto failure;
	}

	if (arr_poll != NULL)
	{
		tmp_buf = bsearch(&fd, arr_poll, size_arr_poll,
						  sizeof(struct pollfd), compare_int_and_pollfd);
		if (tmp_buf != NULL)
		{
			log_debug("tmp_bud.fd: %d", tmp_buf->fd);
			tmp_buf->events = fd < 0 ? 0 : events;
			tmp_buf->fd = fd, tmp_buf->revents = 0;
			log_debug("WARNING: fd alredy exist;(fd: %d)", fd);
			ret_f = size_arr_poll -
					(arr_poll + size_arr_poll - 1 - tmp_buf) - 1;
			goto success;
		}
	}

	if (realloc_for_poll(size_arr_poll + 1) != NULL)
	{
		ret_f = size_arr_poll - 1;

		arr_poll[ret_f].fd = fd;
		arr_poll[ret_f].events = events;
		arr_poll[ret_f].revents = 0;
		qsort(arr_poll, size_arr_poll, sizeof(struct pollfd), compare_pollfd);
	}
	else
	{
		log_debug("FAIL: realloc_for_poll: reallocare failed", NULL);
		ret_f = -1;
		goto failure;
	}

success:
	log_debug("SUCCES: pollfd was added", NULL);
failure:
	return ret_f;
}

/*
 * cleaning_arr_poll -- cleaning @arr_poll from negative fd
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int cleaning_arr_poll(void)
{
	struct pollfd *tmp_arr_poll = (struct pollfd*)malloc(size_arr_poll * sizeof(struct pollfd));
	struct pollfd *tmp_item = NULL;
	nfds_t new_size_arr_poll= 0;
	int ret_f = -1;

	if (tmp_arr_poll != NULL)
	{
		for (tmp_item = arr_poll, new_size_arr_poll = 0;
			 tmp_item < arr_poll + size_arr_poll;
			 ++tmp_item)
		{
			if (tmp_item->fd >= 0)
			{
				memcpy(tmp_arr_poll + new_size_arr_poll++,
					   tmp_item, sizeof(struct pollfd));
			}
		}
		free(arr_poll), arr_poll = tmp_arr_poll;
		realloc_for_poll(new_size_arr_poll);
		ret_f = 0;
	}
	else
	{
		log_err_with_perror("FAIL: malloc: memory for tmp_arr_poll", NULL);
	}

	return ret_f;
}
/*
 * del_pollfd -- deleting fd from @arr_poll
 * @fd -- target fd;
 *
 * return:
 * 0 -- success;
 * -1 -- failure;
 */
int del_pollfd(int fd)
{
	struct pollfd *tmp_ptr = NULL;
	static int del_counter = 0;

	if (arr_poll == NULL || fd < 0)
	{
		log_debug("WARNING: arr_poll == NULL || fd < 0;\n"
				  "(arr_poll: %p, fd: %d)", (void*)arr_poll, fd);
		goto success;
	}

	tmp_ptr = bsearch(&fd, arr_poll, size_arr_poll, sizeof(struct pollfd), compare_int_and_pollfd);
	if (tmp_ptr != NULL)
	{
		tmp_ptr->fd = -fd;
		if (del_counter++ >= RESERV_ITEM)
		{
			del_counter = 0;
			if (cleaning_arr_poll() == -1)
			{
				log_debug("FAIL: cleaning_arr_poll: something wrong", NULL);
				goto failure;
			}
		}
	}

	log_debug("SUCCESS: pollfd was deleted", NULL);
success:
	return 0;
failure:
	return -1;
}

/*
 * add_to_set -- adding fd to according set
 * @result -- result structure of poll_handler();
 * @id -- set id;
 * @fd -- fd which adding to set;
 *
 * return:
 * true -- success
 * false -- failure
 */
bool add_to_set(poll_result_t *result, set_id_t id, int fd)
{
	int **set = NULL, *tmp_set = NULL;
	int counter = 0;

	switch (id)
	{
		case SET_READ: set = &(result->set_read); break;
		case SET_ERR: set = &(result->set_err); break;
		default:
			log_debug("FAIL: bad set_id_t;(id: %u)", id);
			goto failure;
			break;
	}
	counter = result->counter[id];

	tmp_set = (int*)realloc(*set, sizeof(int) * ++counter);
	if (tmp_set != NULL)
	{
		tmp_set[counter - 1] = fd;
		*set = tmp_set, result->counter[id] = counter;
	}
	else
	{
		log_err_with_perror("FAIL: realloc: memory for new set", NULL);
		goto failure;
	}

	return true;
failure:
	return false;
}
/*
 * poll_handler -- proccessing result of poll()
 * @result -- value-result which storage result of poll();
 *
 * return:
 * -1 -- failure;
 * 0 -- poiter to poll_result_t;
 */
int poll_handler(poll_result_t **result)
{
	bool flag = true;
	int amount = 0;
	struct pollfd *tmp_ptr = NULL;

	if (arr_poll == NULL || result == NULL)
	{
		log_debug("WARNING: @arr_poll or @result is empty;\n"
				  "(arr_poll: %p, result: %p)",
				  (void*)arr_poll, (void*)result);
		goto success;
	}

	amount = poll(arr_poll, size_arr_poll, TIMEOUT_POLL);
	if (amount == -1)
	{
		log_err_with_perror("FAIL: poll", NULL);
		goto failure;
	}

	if (amount > 0)
	{
		*result = (poll_result_t*)calloc(1, sizeof(poll_result_t));
		if (*result != NULL)
		{
			(*result)->set_err = (*result)->set_read = NULL;
		}
		else
		{
			log_err_with_perror("FAIL: calloc: memory for result", NULL);
			goto failure;
		}

		for (tmp_ptr = arr_poll, flag = true;
			 (tmp_ptr < arr_poll + size_arr_poll) && (flag == true);
			 ++tmp_ptr)
		{
			switch (tmp_ptr->revents)
			{
				case 0:
					flag = true;
					break;
				case POLLIN:
					flag = add_to_set(*result, SET_READ, tmp_ptr->fd);
					break;
				default:
					flag = add_to_set(*result, SET_ERR, tmp_ptr->fd);
					break;
			}
			tmp_ptr->revents = 0;
		}

		if (flag == false)
		{
			log_debug("FAIL: add_to_set: fd isn't added", NULL);
			goto failure;
		}
	}

success:
	log_debug("SUCCESS: pollfd amount: %d", amount);
	return 0;
failure:
	return -1;
}

void free_poll_result(poll_result_t *result)
{
	if(result != NULL)
	{
		if(result->set_err != NULL) free(result->set_err);
		if (result->set_read != NULL) free(result->set_read);
		free(result);
	}
}
