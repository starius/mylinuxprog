CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET1 = md
TARGET2 = md1
TARGET3 = mt1
TARGET4 = mdsys
TARGET5 = mdsys2
TARGET6 = mdsysc

obj-m  := $(TARGET1).o $(TARGET2).o $(TARGET3).o $(TARGET4).o $(TARGET5).o $(TARGET6).o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko

