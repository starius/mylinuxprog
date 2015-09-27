CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

TARGET1 = hidden
TARGET2 = restore

VARIANT = 0
SYSCALL = 0
EXTRA_CFLAGS += -D VARIANT=$(VARIANT)  # вариант обработчика
EXTRA_CFLAGS += -D SYSCALL=$(SYSCALL)  # номер системного вызова

all: syscall modules clean
	@echo VARIANT=$(VARIANT) SYSCALL=$(SYSCALL)

obj-m := $(TARGET1).o $(TARGET2).o

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

syscall: syscall.h syscall.c
	$(CC) $(EXTRA_CFLAGS) syscall.c -o syscall

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c
	@rm -f .*.*.cmd *~ *.*~ *.symvers
	@rm -fR .tmp* *.order
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko syscall
