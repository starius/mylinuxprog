#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/types.h>

static int __init hello_init( void ) {
   unsigned long j;
   u64 i;
   j = jiffies; 
   printk( KERN_INFO "jiffies = %lX\n", j );   
   printk( KERN_INFO "HZ value = %d\n", HZ );
   i = get_jiffies_64();
   printk( "jiffies 64-bit = %016llX\n", i );
   return -1;
}

module_init( hello_init );
