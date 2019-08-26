libs := libc libzmq

$(libs:%=driver_%): driver_%:
	$(MAKE) -s -C ./$* VV_driver

$(libs:%=clean_driver_%): clean_driver_%:
	$(MAKE) -s -C ./$* clean
