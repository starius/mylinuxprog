CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD  = $(shell pwd)
CC   = gcc -Wall

obj-m := pamix.o pamixx.o pamixm.o pamixab.o pamixad.o

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

