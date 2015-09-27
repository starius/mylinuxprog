CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
EXTRA_CFLAGS += -std=gnu99

TARGET1 = net_proto
TARGET2 = net_proto2
TARGET3 = net_protox
TARGET4 = trn_proto

obj-m	:= $(TARGET1).o $(TARGET2).o $(TARGET3).o

all: modules clean

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order *.d
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean:  clean	
	@rm -f *.ko
