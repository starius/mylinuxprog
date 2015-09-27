CC = gcc
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET1 = slave
PROGS = inst1 inst2 rem1 rem2

obj-m := $(TARGET1).o

all: progs module clean

progs: $(PROGS)

module:	
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko $(PROGS) 

