VPATH := ../common
include_path := ../common
lib_path := ../common/lib
passenger_depend := passenger.o init_pass.o recv_proc.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -iquote $(include_path) -c $<

%.h:

libs:
	$(MAKE) -C ../common/ libnetwork.a
	$(MAKE) -C ../common/ libinit_peer.a

VV_passenger: $(passenger_depend) libs
	$(CC) $(CFLAGS) -L $(lib_path) $(passenger_depend) -o $@ -linit_peer -lnetwork

clean:
	rm -fv *.o VV_*
