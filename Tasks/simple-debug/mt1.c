#include "md1.h"

static int __init init( void ) {
   printk( "%s\n", test_01() );
   printk( "%s\n", test_02() ); 
   return -1;
}

