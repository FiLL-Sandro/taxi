VPATH := ../

network_obj := wrappers_network.o communication.o
init_peer_obj := init_peer.o $(network_obj)
definitions := LOG_DEBUG_IS_ON -D LOG_ERR_WITH_PERROR_IS_ON
include_path := ../

%.o: %.c %.h logging.h
	echo "Creating----------------------------" $@
	$(CC) $(CFLAGS) -iquote $(include_path) -D $(definitions) -c $<

libnetwork.a: $(network_obj)
	ar cr $@ $^
	mv $@ -t ../lib/

libinit_peer.a: $(init_peer_obj)
	ar cr $@ $^
	mv $@ -t ../lib/

clean:
	rm -fv *.o
