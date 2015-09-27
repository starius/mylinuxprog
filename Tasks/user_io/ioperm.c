#include "io.h"

int main( int argc, char *argv[] ) {
   unsigned long addr = PARPORT_BASE; 
   if( argc > 1 ) addr = strtoul( argv[ 1 ], NULL, 0 );
   do_io( addr );
   return errno;
}
