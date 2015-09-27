#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define PARPORT_BASE 0x378

void do_io( unsigned long addr ) {
   unsigned char zero = 0, readout = 0;
   printf( "\twriting: 0x%02x to 0x%lx\n", zero, addr );
   outb( zero, addr );
   usleep( 1000 );
   readout = inb( addr + 1 );
   printf( "\treading: 0x%02x from 0x%lx\n", readout, addr + 1 );
}
