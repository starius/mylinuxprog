CC = gcc -Wall `pkg-config fuse --cflags --libs`
PROGS = hello cusexmp fsel fselclient xmp fusexmp fusexmp_fh

%: %.c
	$(CC) $< -o $@

all:    $(PROGS)

fusexmp_fh: fusexmp_fh.c
	$(CC) $< -lulockmgr -o $@

clean:
	rm -f $(PROGS) *.o
