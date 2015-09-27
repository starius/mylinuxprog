#include "user.h"
#include "common.h"

#ifdef PROC
#define BASE "/proc"
#else
#define BASE "/sys/kernel"
#endif

enum point { VALUE = 0, INCREMENT, MULTIPLY };

static void ctrl( int index ) {
   if( index < VALUE || index > MULTIPLY ) {
      printf( "недопустимый индекс %d\n", index );
      exit( EXIT_FAILURE );
   }
};

static char* read_string( int index ) {
   static char buf[ MAX_LEN ];
   char title[ MAX_LEN ];
   int fd, n;
   ctrl( index );
   sprintf( title, BASE"/%s/%s", directory, points[ index ] );
   if( ( fd  = open( title, O_RDONLY ) ) < 0 ) {
      printf( "ошибка открытия %s: %m\n", title );
      exit( EXIT_FAILURE );
   }
   n = read( fd, buf, MAX_LEN );
   if( n <= 0 ) {
      printf( "ошибка чтения %s: %m\n", title );
      exit( EXIT_FAILURE );
   }
   if( '\n' == buf[ n - 1 ] ) buf[ n - 1 ] = '\0';
   else buf[ n ] = '\0';
   close( fd );
   return buf;
}

static void write_string( int index, long val ) {
   char buf[ MAX_LEN ];
   char title[ MAX_LEN ];
   int fd, n;
   ctrl( index );
   sprintf( title, BASE"/%s/%s", directory, points[ index ] );
   if( ( fd  = open( title, O_WRONLY ) ) < 0 ) {
      printf( "ошибка открытия %s: %m\n", buf );
      exit( EXIT_FAILURE );
   }
   sprintf( buf, "%ld", val );
   n = write( fd, buf, strlen( buf ) + 1 );
   if( n <= 0 ) {
      printf( "ошибка записи %s: %m\n", title );
      exit( EXIT_FAILURE );
   }
   close( fd );
}

long get_value( void ) { return atol( read_string( VALUE ) ); }

void set_value( long new ) { write_string( VALUE, new ); }

int get_multy( void ) { return atol( read_string( MULTIPLY ) ); }

void set_multy( int new ) { write_string( MULTIPLY, (long)new ); }

void do_step( int step ) { write_string( INCREMENT, (long)step ); }
