CFLAGS := -std=gnu11 -O2 -pedantic -Wall -Wextra -Wno-format-extra-args\
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

all: $(patsubst %,VV_%,$(parts))
clean_all: $(patsubst %,clean_%,$(parts))

$(parts:%=VV_%):
	$(MAKE) -C $(patsubst VV_%,%,$@) $@

$(parts:%=clean_%):
	$(MAKE) -C $(patsubst clean_%,%,$@) clean
	$(MAKE) -C common/ clean
