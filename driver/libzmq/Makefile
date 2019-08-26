path_to_common := ../../common
definitions := LOG_DEBUG_IS_ON -D LOG_ERR_WITH_PERROR_IS_ON

VPATH := $(path_to_common):$(path_to_common)/libzmq
include_path := $(path_to_common)
lib_path := $(path_to_common)/lib -L.

driver_obj := driver.o init_driver.o recv_proc.o

%.h:
%.c: %.h
%.o: %.c %.h logging.h
	echo "Creating----------------------------" $@
	$(CC) $(CFLAGS) -D $(definitions) -iquote $(include_path) -c $<

network_libzmq init_peer_libzmq:
	$(MAKE) -s -C $(path_to_common) $@

libs: network_libzmq init_peer_libzmq

VV_driver: $(driver_obj) libs
	$(CC) $(CFLAGS) -L $(lib_path) $(driver_obj) -o $@ -linit_peer -lnetwork -lm -lzmq

clean:
	rm -fv *.o VV_*
