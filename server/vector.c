#include "vector.h"

#define RESERV_ITEM 5

static int cleaning_vector(vec_i_t **vec, u_int* size_vec);

/*
 * add_item_vec -- adding item into @vec
 * @vec -- target vector;
 * @size_vec -- size of @vec expressioned by member amount of @vec
 * @data -- data for item
 * @index -- item index into which writing @data
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
*/
int add_item_vec(vec_i_t **vec, u_int *size_vec, void *data, u_int index)
{
	vec_i_t *new_vec = NULL, *tmp_vec_i = NULL;
	u_int tmp_size = 0;

	if (size_vec == NULL || vec == NULL)
	{
		log_debug("WARNING: size_vec == NULL || vec == NULL;\n"
				  "(size_vec: %p, vec: %p)",
				  (void*)size_vec, (void*)vec);
		goto failure;
	}

	if (index >= *size_vec)
	{
		tmp_size = index + RESERV_ITEM;
		new_vec = (vec_i_t*)realloc(*vec, tmp_size * sizeof(vec_i_t));
		if (new_vec == NULL)
		{
			log_err_with_perror("FAIL: realloc: memory for new_vec", NULL);
			goto failure;
		}

		for (tmp_vec_i = new_vec + *size_vec;
			 tmp_vec_i != new_vec + tmp_size;
			 ++tmp_vec_i)
		{
			tmp_vec_i->is_enable = false;
		}

		*vec = new_vec, *size_vec = tmp_size;
	}

	(*vec)[index].is_enable = true;
	(*vec)[index].data = data;

	log_debug("SUCCESS: item was added into vector", NULL);

	return 0;
failure:
	return -1;
}

/*
 * cleaning_vector -- cleaning @vec from items disabled
 * @vec -- target vector;
 * @size_vec -- size of @vec expressioned by member amount of @vec;
 *
 * return:
 * -1 -- failure;
 * 0 -- success;
 */
int cleaning_vector(vec_i_t** vec, u_int* size_vec)
{
	vec_i_t *tmp_i = NULL;
	u_int tmp_size = 0;

	for (tmp_i = *vec + *size_vec - 1, tmp_size = *size_vec;
		 tmp_i->is_enable == false && tmp_size > 0;
		 --tmp_i, --tmp_size);

	if (*size_vec != tmp_size)
	{
		tmp_i = (vec_i_t*)realloc(*vec, tmp_size * sizeof(vec_i_t));
		if (tmp_i != NULL)
		{
			*vec = tmp_i, *size_vec = tmp_size;
		}
		else
		{
			goto failure;
		}
	}

success:
	return 0;
failure:
	return -1;
}

/*
 * del_item_vec -- item of vector deletion
 * @vec -- target vector;
 * @size_vec -- size of @vec expressioned by member amount of @vec;
 * @index -- item index which will deleted
 *
 * return:
 * -1 -- failure
 * 0 -- success
 */
int del_item_vec(vec_i_t **vec, u_int *size_vec, u_int index)
{
	static u_int del_count = 0;

	if (size_vec == NULL || vec == NULL)
	{
		log_debug("WARNING: size_vec == NULL || vec == NULL;\n"
				  "(size_vec: %p, vec: %p)",
				  (void*)size_vec, (void*)vec);
		goto failure;
	}
	else if (index >= *size_vec)
	{
		log_debug("FAIL: index >= *size_vec\n"
				  "(*size_vec: %u, index: %u)", *size_vec, index);
		goto failure;
	}
	else if (*vec == NULL)
	{
		goto success;
	}

	(*vec)[index].is_enable = false;
	if (del_count++ >= RESERV_ITEM)
	{
		del_count = 0;
		if (cleaning_vector(vec, size_vec) == -1)
		{
			log_debug("FAIL: cleaning_vector: troubles with cleaning vector", NULL);
			goto failure;
		}
	}

success:
	log_debug("SUCCESS: item was successful deleted", NULL);
	return 0;
failure:
	return -1;
}

/*
 * take_data_vec -- extracting data from item of vector
 * @vec -- target vector;
 * @size_vec -- size of @vec expressioned by member amount of @vec;
 * @index -- target item index
 *
 * return:
 * NULL -- failure or data of item is NULL;
 * not-NULL -- success;
 */
void* take_data_vec(vec_i_t *vec, u_int size_vec, u_int index)
{
	if (index >= size_vec)
	{
		log_debug("WARNING: index >= size_vec;\n"
				  "(size_vec: %u, index: %u)",
				  size_vec, index);
		goto failure;
	}

	return  vec == NULL || vec[index].is_enable == false ? NULL
														 : vec[index].data;
failure:
	return NULL;
}
