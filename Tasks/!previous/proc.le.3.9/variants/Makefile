CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

EXTRA_CFLAGS += -std=gnu99
VARIANT = 0
EXTRA_CFLAGS += -D VARIANT=$(VARIANT)  # вариант обработчика read_proc_t

MODULE = mod_proc
TARGET = $(MODULE)_$(VARIANT)
$(TARGET)-objs := $(MODULE).o
obj-m := $(TARGET).o

PROG = mcat recho rtest

all: module $(PROG) clean
	@echo VARIANT=$(VARIANT)

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

mcat:  mcat.c common.h def_prog.c
recho: recho.c common.h def_prog.c
rtest: rtest.c common.h def_prog.c

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko mcat $(PROG)
