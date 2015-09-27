CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
TARGET = poll

obj-m := $(TARGET).o

all:  prog module clean

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

prog: pcat pecho

pcat:   poll.h user.h pcat.c
	gcc pcat.c -o pcat

pecho:  poll.h user.h pecho.c
	gcc pecho.c -o pecho

clean:
	@rm -f *.o  .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko pcat pecho
