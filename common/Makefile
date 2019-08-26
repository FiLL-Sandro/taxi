parts := libc libzmq
libs := network init_peer

$(parts:%=clean_common_%): clean_common_%:
	$(MAKE) -C ./$* clean

$(libs:%=%_libc): %_libc:
	$(MAKE) -s -C ./libc lib$*.a

$(libs:%=%_libzmq): %_libzmq:
	$(MAKE) -s -C ./libzmq lib$*.a
