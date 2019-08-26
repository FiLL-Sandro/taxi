libs := libc libzmq

$(libs:%=server_%): server_%:
	$(MAKE) -s -C ./$* VV_server

$(libs:%=clean_server_%): clean_server_%:
	$(MAKE) -s -C ./$* clean
