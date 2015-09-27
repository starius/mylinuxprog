CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
TARGET = ioctl_signal

obj-m := $(TARGET).o

all: default ioctl sigreq clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

ioctl:	ioctl.c ioctl.h 
	gcc ioctl.c -o ioctl

sigreq:	sigreq.c 
	gcc sigreq.c -o sigreq

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko ioctl sigreq
