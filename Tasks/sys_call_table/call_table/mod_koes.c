#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

static int nsym = 0;
static unsigned long taddr = 0;

int symb_fn( void* data, const char* sym, struct module* mod, unsigned long addr ) {
   nsym++;
   if( 0 == strcmp( (char*)data, sym ) ) {
      printk( "+ sys_call_table address = %lx\n", addr );
      taddr = addr;
      return 1;
   }
   return 0;
};

static int __init ksys_call_tbl_init( void ) {
   int n = kallsyms_on_each_symbol( symb_fn, (void*)"sys_call_table" ); 
   if( n != 0 ) {
      int i;
      char table[ 120 ] = "sys_call_table : ";
      printk( "+ find in position %d\n", nsym );
      for( i = 0; i < 10; i++ ) {
         unsigned long sa = *( (unsigned long*)taddr + i );
         sprintf( table + strlen( table ), "%p ", (void*)sa );
      }
      printk( "+ %s ...\n", table );
   }
   else printk( "+ symbol not found\n" );
   return -EPERM;
}

module_init( ksys_call_tbl_init );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );


