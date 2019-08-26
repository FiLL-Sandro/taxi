#ifndef SUPER_DB_H
#define SUPER_DB_H

#include "common_data.h"
#include "logging.h"
#include "list.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

extern int add_entry(int sfd, kind_host_t kind, void *data);
extern int del_entry(int sfd);
extern passenger_t* set_session(int *dri_fd);
extern int change_position(int sfd, int *pos);
extern int set_free_peer(int sfd, int *pos);
extern int change_status(int sfd, kind_host_t kind, void *stat);
extern int take_dest_fd(int sfd);

#endif
