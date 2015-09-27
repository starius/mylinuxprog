CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc

EXTRA_CFLAGS += -std=gnu99

TARGET1 = devices
TARGET2 = network
TARGET3 = transmit
TARGET4 = transmit_simple
TARGET5 = receive
TARGET6 = mulnet

obj-m	:= $(TARGET1).o $(TARGET2).o $(TARGET3).o \
           $(TARGET4).o $(TARGET5).o $(TARGET6).o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	cp -v $(TARGET).ko $(DEST)
	/sbin/depmod -v | grep $(TARGET)
	/sbin/insmod $(TARGET).ko
	/sbin/lsmod | grep $(TARGET)

uninstall:
	/sbin/rmmod $(TARGET)
	rm -v $(DEST)/$(TARGET).ko
	/sbin/depmod

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko
