#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"

static void do_own_call( char *str ) {
   int n = syscall( __NR_str_trans, str, strlen( str ) );
   if( n >= 0 )
      printf( "syscall return %d\n", n );
   else
      printf( "syscall error %d : %s\n", n, strerror( -n ) );
} 

int main( int argc, char *argv[] ) {
   if( 1 == argc ) do_own_call( "9876" );
   else {
      int i;
      for( i = 1; i < argc; i++ )
         do_own_call( argv[ i ] );
   }
   return EXIT_SUCCESS;
};
