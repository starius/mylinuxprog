CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD  = $(shell pwd)
CC   = gcc -Wall

TARGET0 = nproc
TARGET1 = thr1
TARGET2 = thr2
TARGET3 = thr3
TARGET4 = thr4

obj-m := $(TARGET0).o $(TARGET1).o $(TARGET3).o $(TARGET3).o $(TARGET4).o 
#obj-m := $(TARGET1).o

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

