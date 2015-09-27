CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
MAJOR = 201

TARGET1 = fixdev
TARGET2 = dyndev
obj-m   := $(TARGET1).o $(TARGET2).o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	cp -v $(TARGET).ko $(DEST)
	/sbin/insmod $(TARGET).ko major=$(MAJOR)
	mknod -m0666 /dev/zzzz c $(MAJOR) 0

uninstall:
	/sbin/rmmod $(TARGET)
	rm $(DEST)/$(TARGET).ko
	rm /dev/zzzz

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
