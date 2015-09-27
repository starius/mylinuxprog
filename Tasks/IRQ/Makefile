CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc

TARGET1 = lab1_interrupt
TARGET2 = mod_tasklet
TARGET3 = mod_workqueue
TARGET4 = mod_ser

obj-m	:=	$(TARGET1).o $(TARGET2).o $(TARGET3).o $(TARGET4).o

all: default clean 

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
