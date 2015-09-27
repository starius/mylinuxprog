#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"

static void do_own_call( char *str ) {
   char buf[ 120 ];
   sprintf( buf, "%s\n", str );
   int n = syscall( NR, buf, strlen( buf ) );
   printf( "syscall return %d [%08x], reason: %s\n",
           n, n, strerror( n > 0 ? 0 : -n ) );
}

int main( int argc, char *argv[] ) {
   char str[] = "DEFAULT STRING";
   if( 1 == argc ) {
      do_own_call( str );
      return EXIT_SUCCESS;
   }
   while( --argc > 0 ) do_own_call( argv[ argc ] );
   return EXIT_SUCCESS;
};
