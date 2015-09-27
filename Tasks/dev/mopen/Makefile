CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

TARGET = mmopen
obj-m   := $(TARGET).o 

PROGS = pmopen simple

all: module $(PROGS) clean

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

pmopen:	mopen.h pmopen.c
	gcc pmopen.c -o pmopen

simple:	mopen.h simple.c
	gcc simple.c -o simple

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko $(PROGS)
