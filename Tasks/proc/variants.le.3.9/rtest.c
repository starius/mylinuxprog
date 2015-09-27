#include "def_prog.c"

int main( int argc, char *argv[] ) {
   unsigned long len = 0;
   int df, res;
   char beg = (char)FIRST_CHAR;
   if( argc != 2 ) {
      printf( "command error: %s <name in /proc>\n", argv[ 0 ] );
      return EXIT_FAILURE;
   }
   df = def_proc( argv[ 1 ] );
   do {
      char rd;
      res = read( df, &rd, 1 );
      if( 0 == res  ) {
         printf( "считано %ld байт, все значения совпали с ожидаемыми\n", len );
         break;
      }
      if( res < 0 ) { 
         printf( "read device error: %m\n" );
         break;
      }
      if( 1 != res  ) {
         printf( "это что-то совсем непонятное!\n" );
         break;
      }
      len++;
      if( rd != beg ) {
         printf( "несовпадение в позиции %ld : %d => %d\n",
                 len, (int)beg, (int)rd );
         break;
      } 
      beg = (char)( beg++ % 255 );
   } while ( res > 0 );
   close( df );
   return EXIT_SUCCESS;
};
