// The code herein is: Copyright Jerry Cooperstein, 2012
//     URL:    http://www.coopj.com
//     email:  coop@coopj.com
// The code modified by: Oleg Tsiliuric, 2012
//     email:  olej.tsil@gmail.com

/*
 * Accessing I/O Ports From User-Space
 *
 * Look at /proc/ioports to find a free I/O port region.  One
 * possibility to use the first parallel port, usually at 0x378, where
 * you should be able to write a 0 to the register at the base
 * address, and read the next port for status information.
 */
#include "io.h"
/*
  In each method we will:
  1) Clear the data signal -- see parport_pc.h for register info
  2) Sleep for a millisecond
  3) Read the status port
*/

void do_read_devport( unsigned long addr ) {
   unsigned char zero = 0, readout = 0;
   int fd;
   printf( "/dev/port :\n" );
   if( ( fd = open( "/dev/port", O_RDWR ) ) < 0 ) {
      perror( "reading /dev/port method failed" ); return;
   }
   if( addr != lseek( fd, addr, SEEK_SET ) ) {
      perror( "lseek failed" ); close( fd ); return;
   }
   printf( "\twriting: 0x%02x to 0x%lx\n", zero, addr );
   write( fd, &zero, 1 );
   usleep( 1000 );
   read( fd, &readout, 1 );
   printf( "\treading: 0x%02x from 0x%lx\n", readout, addr + 1 );
   close( fd );
   return;
}

void do_ioperm( unsigned long addr ) {
   printf( "ioperm :\n" );
   if( ioperm( addr, 2, 1 ) ) {
      perror( "ioperm failed" ); return;
   }
   do_io( addr );
   if( ioperm( addr, 2, 0 ) ) perror( "ioperm failed" );
   return;
}

int iopl_level = 3;
void do_iopl( unsigned long addr ) {
   printf( "iopl :\n" );
   if( iopl( iopl_level ) ) {
      perror( "iopl failed" ); return;
   }
   do_io( addr );
   if( iopl( 0 ) ) perror( "ioperm failed" );
}

int main( int argc, char *argv[] ) {
   unsigned long addr = PARPORT_BASE; 
   if( argc > 1 )
      if( !sscanf( argv[ 1 ],"%lx", &addr ) ) {
         printf( "illegal address: %s\n", argv[ 1 ] );
         return EXIT_FAILURE;
      };
   if( argc > 2 ) iopl_level = atoi( argv[ 2 ] );
   do_read_devport( addr );
   do_ioperm( addr );
   do_iopl( addr );
   return errno;
}
