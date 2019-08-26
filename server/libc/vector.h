#ifndef VECTOR_H
#define VECTOR_H

#include "logging.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct vec_i
{
	bool is_enable;
	void *data;
} vec_i_t;

extern int add_item_vec(vec_i_t **vec, u_int* size_vec, void *data, u_int index);
extern int del_item_vec(vec_i_t **vec, u_int* size_vec, u_int index);
extern void* take_data_vec(vec_i_t* vec, u_int size_vec, u_int index);
#endif
