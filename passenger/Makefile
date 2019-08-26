libs := libc libzmq

$(libs:%=passenger_%): passenger_%:
	$(MAKE) -s -C ./$* VV_passenger

$(libs:%=clean_passenger_%): clean_passenger_%:
	$(MAKE) -s -C ./$* clean
