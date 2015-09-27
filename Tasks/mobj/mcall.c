#include <linux/module.h>
#include "mf.h"

static int __init init_driver( void ) {
   printk( "call: %s[%p]\n", mod_func_1(), (void*)mod_func_1 );
   printk( "call: %s[%p]\n", mod_func_2(), (void*)mod_func_2 );
   printk( "call: %s[%p]\n", mod_func_3(), (void*)mod_func_3 );
   return -1;
}

module_init( init_driver );
