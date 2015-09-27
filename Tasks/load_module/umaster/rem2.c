#include "common.h"

int main( int argc, char *argv[] ) {
   char file[ 80 ]  = SLAVE_FILE;
   int res;

   if( argc > 1 ) strcpy( file, argv[ 1 ] );
   char *slave_mod = strrchr( file, '/' ) != NULL ?
                     strrchr( file, '/' ) + 1 :
                     file;
   if( strrchr( file, '.' ) != NULL )
      *strrchr( file, '.' ) = '\0';
   printf( "выгружается модуль %s\n", slave_mod );
   res = delete_module( slave_mod, O_TRUNC );  // flags: O_TRUNC, O_NONBLOCK
   if( res < 0 ) printf( "ошибка выгрузки: %m\n" );
   else printf( "модуль %s успешно загружен!\n", slave_mod );
   return res; 
};
