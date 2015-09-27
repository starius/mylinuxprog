#include "def_prog.c"

int main( int argc, char *argv[] ) {
   unsigned long len, l;
   int df;
   char beg = (char)FIRST_CHAR;
   if( argc != 3 ) {
      printf( "command error: %s <bytes for write> <name in /proc>\n", argv[ 0 ] );
      return EXIT_FAILURE;
   }
   len = atoi( argv[ 1 ] );
   df = def_proc( argv[ 2 ] );
   char *msg = malloc( len );
   if( !msg ) {
      printf( "can't allocate %ld bytes buffer\n", len );
      return EXIT_FAILURE;
   }
   for( l = 0; l < len; l++ )
      *(char*)( msg + l ) = beg++ % 255;
   if( ( l = write( df, msg, len ) ) != len ) 
      printf( "write error: %m\n" );
   free( msg );
   close( df );
   return EXIT_SUCCESS;
};
