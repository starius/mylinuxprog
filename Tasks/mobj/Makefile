CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

EXTRA_CFLAGS += -std=gnu89 --no-warnings

TARGET = mobj
TARGET2 = mcall

OBJS = mod.o mfA.o mfB.o mfC.o
$(TARGET)-objs := $(OBJS)
obj-m      := $(TARGET).o $(TARGET2).o

all:    modules clean

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

$(TARGET).o: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c
	@rm -f .*.*.cmd *~ *.*~
	@rm -fR .tmp* *.order
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko *.symvers
