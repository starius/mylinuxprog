/*
 * (c) Copyright Jerry Cooperstein, 2009
 * (c) Copyright Oleg Tsiliuric, 2011-2014
 */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>

static int num = 1;               // число создаваемых интерфейсов
module_param( num, int, 0 );
static char* title;               // префикс имени интерфейсов
module_param( title, charp, 0 );
static int digit = 1;             // числовые суфиксы (по умолчанию)
module_param( digit, int, 0 );
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0))
static int mode = 1;              // режим нумерации интерфейсов
module_param( mode, int, 0 );
#endif

static struct net_device *adev[] = { NULL, NULL, NULL, NULL };

static int my_open( struct net_device *dev ) {
   printk( KERN_INFO "! my_open(%s)\n", dev->name );
   /* start up the transmission queue */
   netif_start_queue( dev );
   return 0;
}

static int my_close( struct net_device *dev ) {
   printk( KERN_INFO "! my_close(%s)\n", dev->name );
   /* shutdown the transmission queue */
   netif_stop_queue( dev );
   return 0;
}

/* Note this method is only needed on some; without it
   module will fail upon removal or use. At any rate there is a memory
   leak whenever you try to send a packet through in any case*/
static int stub_start_xmit( struct sk_buff *skb, struct net_device *dev ) {
   dev_kfree_skb( skb );
   return 0;
}

static struct net_device_ops ndo = {
   .ndo_open = my_open,
   .ndo_stop = my_close,
   .ndo_start_xmit = stub_start_xmit,
};

static int ipos;

static void __init my_setup( struct net_device *dev ) {
   /* Fill in the MAC address with a phoney */
   int j;
   for( j = 0; j < ETH_ALEN; ++j ) 
      dev->dev_addr[ j ] = (char)( j + ipos );
   ether_setup( dev );
   dev->netdev_ops = &ndo;
}

static int __init my_init( void ) {
   char prefix[ 20 ];
   if( num > sizeof( adev ) / sizeof( adev[ 0 ] ) ) {
      printk( KERN_INFO "! link number error" );
      return -EINVAL;
   }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0))
   if( mode < 0 || mode > NET_NAME_RENAMED ) {
      printk( KERN_INFO "! unknown name assign mode" );
      return -EINVAL;
   }
#endif
   printk( KERN_INFO "! loading network module for %d links", num );
   sprintf( prefix, "%s%s", ( NULL == title ? "fict" : title ), "%d" );
   for( ipos = 0; ipos < num; ipos++ ) {
      if( !digit )
         sprintf( prefix, "%s%c", ( NULL == title ? "fict" : title ), 'a' + ipos );
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0))
      adev[ ipos ] = alloc_netdev( 0, prefix, my_setup );
#else
// NET_NAME_UNKNOWN, NET_NAME_ENUM, NET_NAME_PREDICTABLE, NET_NAME_USER, NET_NAME_RENAMED 
      adev[ ipos ] = alloc_netdev( 0, prefix, NET_NAME_UNKNOWN + mode, my_setup );
#endif
      if( register_netdev( adev[ ipos ] ) ) {
         int j;
         printk( KERN_INFO "! failed to register" );
         for( j = ipos; j >= 0; j-- ) {
            if( j != ipos ) unregister_netdev( adev[ ipos ] );
            free_netdev( adev[ ipos ] );
         }
         return -ELNRNG;
      }
   }
   printk( KERN_INFO "! succeeded in loading %d devices!", num );
   return 0;
}

static void __exit my_exit( void ) {
   int i;
   printk( KERN_INFO "! unloading network module" );
   for( i = 0; i < num; i++ ) {
      unregister_netdev( adev[ i ] );
      free_netdev( adev[ i ] );
   }
} 

module_init( my_init );
module_exit( my_exit );

MODULE_AUTHOR( "Bill Shubert" );
MODULE_AUTHOR( "Jerry Cooperstein" );
MODULE_AUTHOR( "Tatsuo Kawasaki" );
MODULE_AUTHOR( "Oleg Tsiliuric" );
MODULE_DESCRIPTION( "LDD:1.0 s_24/lab1_network.c" );
MODULE_LICENSE( "GPL v2" );
