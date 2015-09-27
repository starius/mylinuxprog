#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mopen.h"

int main( int argc, char *argv[] ) {
   char dev[ 80 ] = "/dev/";
   strcat( dev, DEVNAM );
   int df;
   if( ( df = open( dev, O_RDWR ) ) < 0 )
      printf( "open device error: %m\n" ), exit( EXIT_FAILURE );
   char msg[ 160 ];
   fprintf( stdout, "> " );
   fflush( stdout );   
   gets( msg ); // gets() - опасная функция!
   int res, len = strlen( msg );
   if( ( res = write( df, msg, len ) ) != len )
      printf( "write device error: %m\n" );
   char *p = msg;
   do {
      if( ( res = read( df, p, sizeof( msg ) ) ) > 0 ) {
         *( p + res ) = '\0';
         printf( "read %d bytes: %s", res, p );
         p += res;
      }
      else if( res < 0 )
         printf( "read device error: %m\n" );
   } while ( res > 0 );
   fprintf( stdout, "%s", msg );
   close( df );
   return EXIT_SUCCESS;
};
