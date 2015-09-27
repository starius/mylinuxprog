#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>

static unsigned long asct = 0, anif = 0;

int symb_fn( void* data, const char* sym, struct module* mod, unsigned long addr ) {
   if( 0 == strcmp( "sys_call_table", sym ) )
      asct = addr;
   else if( 0 == strcmp( "sys_ni_syscall", sym ) ) 
      anif = addr;
   return 0;
}

#define SYS_NR_MAX_OLD 340  
// - этот размер таблицы взят довольно произвольно из ядра 2.6.37
static void show_entries( void ) {
   int i, ni = 0;
   char buf[ 200 ] = "";
   for( i = 0; i <= SYS_NR_MAX_OLD; i++ ) {
      unsigned long *taddr = ((unsigned long*)asct) + i;
      if( *taddr == anif ) {
         ni++;
         sprintf( buf + strlen( buf ), "%03d, ", i );
      } 
   }
   printk( "! найдено %d входов: %s\n", ni, buf );
}

static int __init init_driver( void ) {
   kallsyms_on_each_symbol( symb_fn, NULL ); 
   printk( "! адрес таблицы системных вызовов = %lx\n", asct );
   printk( "! адрес не реализованных вызовов  = %lx\n", anif );
   if( 0 == asct || 0 == anif ) { 
      printk( "! не найдены символы ядра\n" );
      return -EFAULT;
   }
   show_entries();
   return -EPERM;
}

module_init( init_driver );

MODULE_DESCRIPTION( "test unused entries" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_LICENSE( "GPL" );
