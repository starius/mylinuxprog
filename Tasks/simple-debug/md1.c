#include "md1.h"

static char retpref[] = "this string returned from ";

char* test_01( void ) {
   static char res[ 80 ];
   strcpy( res, retpref );
   strcat( res, __FUNCTION__ );
   return res;
};
EXPORT_SYMBOL( test_01 );

char* test_02( void ) {
   static char res[ 80 ];
   strcpy( res, retpref );
   strcat( res, __FUNCTION__ );
   return res;
};
EXPORT_SYMBOL( test_02 );

static int __init init( void ) {
   return 0;
}

static void __exit cleanup( void ) {}
module_exit( cleanup );

