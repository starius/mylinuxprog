CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
DEVPATH = /dev/ioctl

TARGET = ioctl_dev
obj-m  := $(TARGET).o

all: default ioctl clean

default: $(TARGET).c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

ioctl:	ioctl.h ioctl.c
	gcc ioctl.c -o ioctl

install:
	cp -v $(TARGET).ko $(DEST)
	/sbin/depmod -v | grep $(TARGET)
	mknod -m0666 $(DEVPATH) c 200 0
	/sbin/insmod $(TARGET).ko
	/sbin/lsmod | grep $(TARGET)

uninstall:
	/sbin/rmmod $(TARGET)
	rm -v $(DEST)/$(TARGET).ko
	/sbin/depmod
	rm -f $(DEVPATH)

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko ioctl
