#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/smp.h>

short data_percpu[] = { 0, 0, 0, 0, 0, 0, 0 ,0 };
int size = sizeof( data_percpu ) / sizeof( data_percpu[ 0 ] );

void call_func( void* info ) {
   int cpu = get_cpu();
   if( cpu >= size ) {
      printk( KERN_INFO "CPU number %d grater %d\n", cpu, size ); 
      return;
   }
   data_percpu[ cpu ]++;
   put_cpu();
}

int test_nump( void ) {
   char msg[ 120 ] = ""; // = "CPU in use: "
   int i;
   on_each_cpu( call_func, NULL, 1 );
   for( i = 0; i < size; i++ ) 
      if( data_percpu[ i ] )
         sprintf( msg + strlen( msg ), "%d ", i ); 
   printk( KERN_INFO "CPU in use: %s\n", msg ); 
   return -1; 
} 

module_init( test_nump ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
