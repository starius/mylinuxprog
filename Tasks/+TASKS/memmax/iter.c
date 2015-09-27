#include <stdio.h>
#include <string.h>

typedef unsigned long long data_t;

data_t val;

inline int ok( data_t lim ) {
   return lim < val;     // здесь может быть любой критерий
}

data_t request( void ) {
   char buf[ 80 ], ctl[ 80 ];
   unsigned long long targ; 
   int n = 0;
   for(;;) {
      fprintf( stdout, "цель (число): " );
      fflush( stdout );
      fgets( buf, sizeof( buf ) - 1, stdin );
      buf[ strlen( buf ) - 1 ] = '\0';  // удалить \n
      n = sscanf( buf, "%llu%s", &targ, ctl );
      if( n != 1 ) {
         printf( "ошибка ввода!\n" );
         goto err;
      }
      sprintf( ctl, "%llu", targ );
      if( strcmp( buf, ctl ) != 0 ) {
         printf( "превышение размерности!\n" );
         goto err;
      }
      break;
   err:;
   };
   printf( "введена строка\t%s\nзадана цель\t%llu\n", buf, targ );
   return (data_t)targ;
}

int main( int argc, char **argv, char **envp ) {
   while( 1 ) {
      data_t liml = 0, limh = 1;
      val = request();
      while( 1 ) {     // найти верхнюю границу
         printf( "%llu | ", (unsigned long long)limh );
         if( !ok( limh ) ) break;
         limh *= 2;
      }
      liml = limh / 2;      
      printf( "\n" );
      while( 1 ) {     // сужение границ 
         data_t midle = ( limh + liml ) / 2;
         printf( "%llu - %llu | ", (unsigned long long)liml, (unsigned long long)limh );
         if( limh == val || ( limh - liml ) < 2 ) break;
         if( ok( midle ) ) liml = midle;
         else limh = midle;
      }
      printf( "\nзадана цель\t%llu\n", val );
      printf( "найдено число\t%llu\n", (unsigned long long)limh );
   }
   return 0;
}
