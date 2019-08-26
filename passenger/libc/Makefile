path_to_common := ../../common
definitions := LOG_DEBUG_IS_ON -D LOG_ERR_WITH_PERROR_IS_ON

VPATH := $(path_to_common):$(path_to_common)/libc
include_path := $(path_to_common) -iquote $(path_to_common)/libc
lib_path := $(path_to_common)/lib -L.

passenger_obj := passenger.o init_pass.o recv_proc.o

%.h:
%.c: %.h
%.o: %.c %.h logging.h
	echo "Creating----------------------------" $@
	$(CC) $(CFLAGS) -D $(definitions) -iquote $(include_path) -c $<

network_libc init_peer_libc:
	$(MAKE) -s -C $(path_to_common) $@

libs: network_libc init_peer_libc

VV_passenger: $(passenger_obj) libs
	$(CC) $(CFLAGS) -L $(lib_path) $(passenger_obj) -o $@ -linit_peer -lnetwork

clean:
	rm -fv *.o VV_*
