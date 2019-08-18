libnetwork_depend := wrappers_network.o communication.o
libinit_peer_depend := init_peer.o

%.c: %.h
%.o: %.c
	$(CC) $(CFLAGS) -c $<

libnetwork.a: $(libnetwork_depend) logging.h
	ar rc $@ $(libnetwork_depend)
	mv $@ -t lib/

libinit_peer.a: $(libinit_peer_depend) logging.h
	ar rc $@ $(libinit_peer_depend)
	mv $@ -t lib/

clean:
	rm -fv *.o ./lib/*
