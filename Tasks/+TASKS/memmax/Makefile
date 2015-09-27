CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
CC = gcc -Wall

TARGET1 = memmax

obj-m  := $(TARGET1).o 

all: module iter clean

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

iter:   iter.c
	$(CC) iter.c -o iter
 
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko iter
