CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
CC = gcc -Wall -Wno-unused-variable
EXTRA_CFLAGS += -std=gnu99
PROGLIST = cli_locl cli_krnp cli_krns

TARGET1 = modpasu
TARGET2 = modsasu
obj-m  := $(TARGET1).o $(TARGET2).o
 
all: module clean progs clean

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

progs:  $(PROGLIST)

cli_locl: cli.c locl.c common.h
	$(CC) -lm cli.c locl.c -o $@

cli_krnp: cli.c kern.c common.h
	$(CC) -DPROC -lm cli.c kern.c -o $@

cli_krns: cli.c kern.c common.h
	$(CC) -DSYS -lm cli.c kern.c -o $@

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko $(PROGLIST)

