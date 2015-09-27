#include "user.h"

int main( int argc, char **argv, char **envp ) {
   int c, n, mdelay = 300, nlen = 10;;
   char sopt[] = "l:d:";
   while( -1 != ( c = getopt( argc, argv, sopt ) ) )
      switch( c ) {
         case 'd':
            mdelay = atoi( optarg );
            break;
         case 'l':
            nlen = atoi( optarg );
            break;
         default :
            printf( "допустимые опции: %s\n", sopt );
            return( EXIT_FAILURE );
      } 
   while( 1 ) {
      char buf[ MAX_LEN ];
      int n = 0;
      fprintf( stdout, "команда (h-подсказка): " );
      fflush( stdout );
      fgets( buf, sizeof( buf ) - 1, stdin );
      switch( buf[ 0 ] ) {
         case '*' : {                 // петлевое усиление
               int ku = 0;
               sscanf( buf + 1, "%d", &ku );
               if( ku != 0 ) set_multy( ku );
               printf( "усиление = %d%%\n", get_multy() ); 
            } continue;
         case '?' : 
            printf( "текущее значение = %ld\n", get_value() );
            continue;
         case '=' : {                 // возмущение
               long new_val = 0;
               sscanf( buf + 1, "%ld", &new_val );
               if( new_val != 0 ) set_value( new_val );
               printf( "новое значение = %ld\n", get_value() );
               if( new_val != 0 ) break;
               else continue;
            };
         case '!' : {
               int new_val = 0;       // длина последовательности
               sscanf( buf + 1, "%u", &new_val );
               if( new_val != 0 ) nlen = new_val;
               printf( "продолжительность = %d циклов\n", nlen );
            }
            continue;
         case 'q' : 
            return( EXIT_SUCCESS );
         default  :
            printf( "нераспознанная команда!\n" );
         case 'h' : 
            printf( "реализованные команды: ? =N *[N] ![N] h q\n" );
            continue;
      }
      c = 1;
      while( 1 ) {
         struct timespec msec;        // пассивная задержка в msec.
         long cur_val = get_value();
         int istep = -(int)cur_val;
         if( abs( cur_val ) < c || ++n > nlen ) break;
         if( 1 == abs( cur_val ) ) c = 2; 
         do_step( istep );            // выполнить шаг регулирования
         msec.tv_sec = 0; 
         msec.tv_nsec = mdelay * 1000000L;
         nanosleep( &msec, NULL ); 
         printf( "%ld %+d => %ld\n", cur_val, istep, get_value() );
      }
   }
}
