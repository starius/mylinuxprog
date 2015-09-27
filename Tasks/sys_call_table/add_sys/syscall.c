#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"

static void do_own_call( char *str ) {
   int n = syscall( __NR_own, str, strlen( str ) );
   if( n >= 0 )
      printf( "syscall return %d\n", n );
   else
      printf( "syscall error %d : %s\n", n, strerror( -n ) );
}

int main( int argc, char *argv[] ) {
   char str[] = "DEFAULT STRING";
   if( 1 == argc ) do_own_call( str );
   else
      while( --argc > 0 ) do_own_call( argv[ argc ] );
   return EXIT_SUCCESS;
};

