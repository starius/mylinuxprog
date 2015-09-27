#include <sys/stat.h>
#include <errno.h>
#include "common.h"

int main( int argc, char *argv[] ) {
   char parms[ 80 ] = "", file[ 80 ] = SLAVE_FILE;
   void *buff = NULL;
   int fd, res;
   off_t fsize;         /* общий размер в байтах */
   if( argc > 1 ) {
      strcpy( file, argv[ 1 ] );
      if( argc > 2 ) {
         int i;
         for( i = 2; i < argc; i++ ) {
            strcat( parms, argv[ i ] );
            strcat( parms, " " );
         } 
      }
   }
   printf( "загрузка модуля: %s %s\n", file, parms );
   fd = open( file, O_RDONLY );
   if( fd < 0 ) {
      printf( "ошибка open: %m\n" );
      return errno;
   }
   {  struct stat fst;
      if( fstat( fd, &fst ) < 0 ) {
         printf( "ошибка stat: %m\n" );
         close( fd );
         return errno;
     }
     if( !S_ISREG( fst.st_mode ) ) {
         printf( "ошибка: %s не файл\n", file );
         close( fd );
         return EXIT_FAILURE;
     }
     fsize = fst.st_size;
   }
   printf( "размер файла модуля %s = %ld байт\n", file, fsize );
   buff = malloc( fsize );
   if( NULL == buff ) {
      printf( "ошибка malloc: %m\n" );
      close( fd );
      return errno;
   }
   if( fsize != read( fd, buff, fsize ) ) {
      printf( "ошибка read: %m\n" );
      free( buff );
      close( fd );
      return errno;
   }
   close( fd );
   res = init_module( buff, fsize, parms ); // вызов sys_init_module()
   free( buff );
   if( res < 0 ) printf( "ошибка загрузки: %m\n" );
   else printf( "модуль %s успешно загружен!\n", file );
   return res; 
};

