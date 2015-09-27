CC = gcc -Wall
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

TARGET1 = mdu
TARGET2 = mdc
TARGET3 = mds
PROGS = mp mpsys mplib

obj-m := $(TARGET1).o $(TARGET2).o $(TARGET3).o

all:  proc modl clean

proc: $(PROGS)

modl:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko $(PROGS)

