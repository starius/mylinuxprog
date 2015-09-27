/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>

static int __init my_init( void ) {
   struct net_device *dev;
   printk( KERN_INFO "Hello: module loaded at 0x%p\n", my_init );
   dev = first_net_device( &init_net );
   printk( KERN_INFO "Hello: dev_base address=0x%p\n", dev );
   while ( dev ) {
      printk( KERN_INFO
              "name = %6s irq=%4d trans_start=%12lu last_rx=%12lu\n",
              dev->name, dev->irq, dev->trans_start, dev->last_rx );
      dev = next_net_device( dev );
   }
   return -1;
}

module_init( my_init );

MODULE_AUTHOR( "Jerry Cooperstein" );
MODULE_DESCRIPTION( "LDD:1.0 s_25/lab1_devices.c" );
MODULE_LICENSE( "GPL v2" );
