#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/types.h>

static u32 j;

static int __init init( void ) {
   j = jiffies; 
   printk( KERN_INFO "module: jiffies on start = %X\n", j );   
   return 0;
}

void cleanup( void ) {
   static u32 j1;
   j1 = jiffies; 
   printk( KERN_INFO "module: jiffies on finish = %X\n", j1 );   
   j = j1 - j;
   printk( KERN_INFO "module: interval of life = %d\n", j / HZ );   
   return;
}

module_init( init );
module_exit( cleanup );
