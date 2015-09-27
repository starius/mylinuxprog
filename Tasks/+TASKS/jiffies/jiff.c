#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>

int init_module( void ) { 
   printk( KERN_INFO "module: jiffies on start = %lX\n", jiffies );
   return -1;
} 

