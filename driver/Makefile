VPATH := ../common
include_path := ../common
lib_path := ../common/lib
driver_depend := driver.o init_driver.o recv_proc.o

%.c: %.h
%.o: %.c
	$(CC) $(CFLAGS) -iquote $(include_path) -c $<

libs:
	$(MAKE) -C ../common/ libnetwork.a
	$(MAKE) -C ../common/ libinit_peer.a

VV_driver: $(driver_depend) libs
	$(CC) $(CFLAGS) -L $(lib_path) $(driver_depend) -o $@ -linit_peer -lnetwork

clean:
	rm -fv *.o VV_*
