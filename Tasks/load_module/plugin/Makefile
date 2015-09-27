CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET1 = master
TARGET2 = dec
TARGET3 = oct
TARGET4 = hex
PROGS = syscall

obj-m := $(TARGET1).o $(TARGET2).o $(TARGET3).o $(TARGET4).o

all: module $(PROGS) clean

module:	
	$(MAKE) -C $(KDIR) M=$(PWD) modules

syscall: syscall.h syscall.c
	$(CC) syscall.c -o syscall

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd  *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko *.symvers $(PROGS)
