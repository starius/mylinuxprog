CC = gcc -Wall
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

all: mod clean

TARGET1 = jiff
TARGET2 = jifft
obj-m      := $(TARGET1).o $(TARGET2).o

mod:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*

disclean: clean
	@rm -f *.ko 
