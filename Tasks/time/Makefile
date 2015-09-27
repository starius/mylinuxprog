CC = gcc
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

all: mod lib prog clean

# --- модули ядра

TARGET1 = tick
TARGET2 = htick
TARGET3 = interv
obj-m      := $(TARGET1).o $(TARGET2).o $(TARGET3).o 

mod:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# --- статическая библиотека 

LIBTITLE = diag
LIBRARY = lib$(LIBTITLE).a
LIBHEAD = lib$(LIBTITLE).h
OBJLIST = calibr.o rdtsc.o proc_hz.o set_rt.o
TXTLIST = calibr.c rdtsc.c proc_hz.c set_rt.c

#lib:	$(OBJLIST) 
lib:	$(TXTLIST) 
	make $(OBJLIST)
	ar -t $(LIBRARY)
	make clean

calibr.o: calibr.c
	$(CC) -c $< -o $@ 
	ar -r $(LIBRARY) $@        

rdtsc.o: rdtsc.c
	$(CC) -c $< -o $@ 
	ar -r $(LIBRARY) $@        

proc_hz.o: proc_hz.c
	$(CC) -c $< -o $@ 
	ar -r $(LIBRARY) $@        

set_rt.o: set_rt.c
	$(CC) -c $< -o $@ 
	ar -r $(LIBRARY) $@        

# --- тестовые программы

PROGLIST = clock pdelay rtcr rtprd 
prog:	$(PROGLIST) $(TXTLIST) 

clock:  clock.c $(LIBRARY)
	$(CC) $< -Bstatic -L./ -l$(LIBTITLE) -o $@

pdelay:  pdelay.c $(LIBRARY)
	$(CC) $< -Bstatic -L./ -l$(LIBTITLE) -o $@

rtprd:	rtprd.c $(LIBRARY)
	$(CC) $< -Bstatic -L./ -l$(LIBTITLE) -o $@

# --- очистки 

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko $(PROGLIST) $(LIBRARY)
