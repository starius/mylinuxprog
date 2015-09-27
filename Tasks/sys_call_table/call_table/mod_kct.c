#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

static int __init ksys_call_tbl_init( void ) {
   void** sct = (void**)kallsyms_lookup_name( "sys_call_table" );
   printk( "+ sys_call_table address = %p\n", sct );
   if( sct ) {
      int i;
      char table[ 120 ] = "sys_call_table : ";
      for( i = 0; i < 10; i++ ) 
         sprintf( table + strlen( table ), "%p ", sct[ i ] );
      printk( "+ %s ...\n", table );
   }
   return -EPERM;
}

module_init( ksys_call_tbl_init );
MODULE_LICENSE( "GPL" );

