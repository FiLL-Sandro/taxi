path_to_common := ../../common
definitions := LOG_DEBUG_IS_ON -D LOG_ERR_WITH_PERROR_IS_ON

VPATH := $(path_to_common):$(path_to_common)/libc
include_path := $(path_to_common) -iquote $(path_to_common)/libc
lib_path := $(path_to_common)/lib -L.

server_obj := server.o init_server.o receiving.o \
	proccessing.o poll_oper.o que_msg.o init_session.o
libdb_obj := vector.o list.o super_db.o

%.h:
%.c: %.h
%.o: %.c %.h logging.h
	echo "Creating----------------------------" $@
	$(CC) $(CFLAGS) -D $(definitions) -iquote $(include_path) -c $<

network_libc:
	$(MAKE) -s -C $(path_to_common) $@

libdb.a: $(libdb_obj)
	echo "Creating----------------------------" $@
	ar rc $@ $^

libs: network_libc libdb.a

VV_server: $(server_obj) libs
	echo "Creating----------------------------" $@
	$(CC) $(CFLAGS) -L $(lib_path) $(server_obj) -o $@ -lnetwork -ldb -lm -lpthread

clean:
	rm -fv *.o VV_* lib*.a
