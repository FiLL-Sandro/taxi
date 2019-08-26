CFLAGS := -std=gnu11 -O2 -pedantic -Wall -Wextra -Wno-format-extra-args \
	-g3 -ggdb3 -gdwarf-4 -fvar-tracking-assignments \
	-fstack-check -fstack-protector-all -ftrapv -D_FORTIFY_SOURCE=2 -fPIE -pie \
	-Wl,-z,relro,-z,now -Wl,-z,noexecstack \
	-Wbad-function-cast -Wcast-align -Wcast-qual -Wconversion -Wsign-conversion \
	-Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness \
	-Winline -Winvalid-pch -Wjump-misses-init -Wlogical-op -Wmissing-declarations \
	-Wmissing-include-dirs -Wmissing-prototypes -Wstrict-prototypes -Wnested-externs -Wold-style-definition \
	-Woverlength-strings -Wpacked -Wredundant-decls -Wshadow -Wstrict-overflow=4 \
	-Wswitch-default -Wtrampolines \
	-Wundef -Wunsuffixed-float-constants -Wunused -Wunused-local-typedefs -Wwrite-strings \
	-Wshift-overflow=2 -Wduplicated-cond \
	-fsanitize=address -fsanitize=undefined -fno-sanitize-recover \
	-Werror=implicit-function-declaration -Werror=address

CC := gcc
parts := server passenger driver

export CFLAGS CC
unexport parts

all_libc: $(patsubst %,%_libc,$(parts))
clean_libc: $(patsubst %,clean_%_libc,$(parts))
all_zmq: $(patsubst %,%_libzmq,$(parts))
clean_zmq: $(patsubst %,clean_%_libzmq,$(parts))
all_clean: clean_libc clean_libzmq clean_common_libc clean_common_libzmq

$(parts:%=%_libc): %_libc:
	$(MAKE) -s -C ./$* $@

$(parts:%=clean_%_libc): clean_%_libc:
	$(MAKE) -C ./$* $@

$(parts:%=%_libzmq): %_libzmq:
	$(MAKE) -s -C ./$* $@

$(parts:%=clean_%_libzmq): clean_%_libzmq:
	$(MAKE) -C ./$* $@

clean_common_libc clean_common_libzmq:
	$(MAKE) -C common/ $@
