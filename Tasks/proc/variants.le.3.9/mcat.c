#include "def_prog.c"

int main( int argc, char *argv[] ) {
   int len = ( argc > 1 && atoi( argv[ 1 ] ) > 0 ) ?
             atoi( argv[ 1 ] ) : LEN_MSG,
       df = argc > 2 ? def_proc( argv[ 2 ] ) : get_proc();
   char msg[ LEN_MSG + 1 ] = "";
   char *p = msg;
   int res;
   do {
      if( ( res = read( df, p, len ) ) >= 0 ) {
         *( p += res )  = '\0';
         printf( "read + %02d bytes, input buffer: %s", res, msg );
         if( *( p - 1 ) != '\n' ) printf( "\n" );
      }
      else printf( "read device error: %m\n" );
   } while ( res > 0 );
   close( df );
   return EXIT_SUCCESS;
};
