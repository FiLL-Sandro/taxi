VPATH := ../common
include_path := ../common
lib_path := ../common/lib -L.
server_depend := server.o init_server.o receiving.o proccessing.o poll_oper.o que_msg.o
libdb := vector.o list.o super_db.o

%.c: %.h
%.o: %.c
	$(CC) $(CFLAGS) -iquote $(include_path) -c $<

libs: $(libdb)
	$(MAKE) -C ../common/ libnetwork.a
	ar rc libdb.a $(libdb)

VV_server: $(server_depend) libs
	$(CC) $(CFLAGS) -L $(lib_path) $(server_depend) -o $@ -lnetwork -ldb -lm -lpthread

clean:
	rm -fv *.o VV_* lib*.a
