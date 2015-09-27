#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/inetdevice.h>
#include <linux/moduleparam.h>
#include <net/arp.h>
#include <linux/ip.h>

#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )
#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define DBG(...) if( debug != 0 ) printk( KERN_INFO "! "__VA_ARGS__ )

static char* link = "eth0";
module_param( link, charp, 0 );

static char* ifname = "virt"; 
module_param( ifname, charp, 0 );

static int debug = 0;
module_param( debug, int, 0 );

static struct net_device *child = NULL;
static struct net_device_stats stats;  // статическая таблица статистики интерфейса
static u32 child_ip;

struct priv {
   struct net_device *parent;
};

static char* strIP( u32 addr ) {      // диагностика IP в точечной нотации
   static char saddr[ MAX_ADDR_LEN ];
   sprintf( saddr, "%d.%d.%d.%d",
            ( addr ) & 0xFF, ( addr >> 8 ) & 0xFF,
            ( addr >> 16 ) & 0xFF, ( addr >> 24 ) & 0xFF
          );
   return saddr;
}

static int open( struct net_device *dev ) {
   struct in_device *in_dev = dev->ip_ptr;
   struct in_ifaddr *ifa = in_dev->ifa_list;      /* IP ifaddr chain */
   LOG( "%s: device opened", dev->name );
   child_ip = ifa->ifa_address;
   netif_start_queue( dev );
   if( debug != 0 ) {
      char sdebg[ 40 ] = "";
      sprintf( sdebg, "%s:", strIP( ifa->ifa_address ) );
      strcat( sdebg, strIP( ifa->ifa_mask ) );
      DBG( "%s: %s", dev->name, sdebg );
   }
   return 0;
}

static int stop( struct net_device *dev ) {
   LOG( "%s: device closed", dev->name );
   netif_stop_queue( dev );
   return 0;
}

static struct net_device_stats *get_stats( struct net_device *dev ) {
   return &stats;
}

// передача фрейма
static netdev_tx_t start_xmit( struct sk_buff *skb, struct net_device *dev ) {
   struct priv *priv = netdev_priv( dev );
   stats.tx_packets++;
   stats.tx_bytes += skb->len;
   skb->dev = priv->parent;   // передача в родительский (физический) интерфейс
   skb->priority = 1;
   dev_queue_xmit( skb );
   DBG( "tx: injecting frame from %s to %s with length: %u",
        dev->name, skb->dev->name, skb->len );
   return 0;
   return NETDEV_TX_OK;
}

static struct net_device_ops net_device_ops = {
   .ndo_open = open,
   .ndo_stop = stop,
   .ndo_get_stats = get_stats,
   .ndo_start_xmit = start_xmit,
};

// приём фрейма
int pack_parent( struct sk_buff *skb, struct net_device *dev,
                 struct packet_type *pt, struct net_device *odev ) {
   skb->dev = child;          // передача фрейма в виртуальный интерфейс
   stats.rx_packets++;
   stats.rx_bytes += skb->len;
   DBG( "tx: injecting frame from %s to %s with length: %u",
        dev->name, skb->dev->name, skb->len );
   kfree_skb( skb );
   return skb->len;
};

static struct packet_type proto_parent = {
   __constant_htons( ETH_P_ALL ), // перехватывать все пакеты: ETH_P_ARP & ETH_P_IP 
   NULL,
   pack_parent,
   (void*)1,
   NULL
};

int __init init( void ) {
   void setup( struct net_device *dev ) { // вложенная функция (GCC)
      int j;
      ether_setup( dev );
      memset( netdev_priv( dev ), 0, sizeof( struct priv ) );
      dev->netdev_ops = &net_device_ops;
      for( j = 0; j < ETH_ALEN; ++j )     // заполнить MAC фиктивным адресом 
         dev->dev_addr[ j ] = (char)j;
   }
   int err = 0;
   struct priv *priv;
   char ifstr[ 40 ];
   sprintf( ifstr, "%s%s", ifname, "%d" );
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0))
   child = alloc_netdev( sizeof( struct priv ), ifstr, setup );
#else
   child = alloc_netdev( sizeof( struct priv ), ifstr, NET_NAME_UNKNOWN, setup );
#endif
   if( child == NULL ) {
      ERR( "%s: allocate error", THIS_MODULE->name ); return -ENOMEM;
   }
   priv = netdev_priv( child );
   priv->parent = __dev_get_by_name( &init_net, link ); // родительский интерфейс  
   if( !priv->parent ) {
      ERR( "%s: no such net: %s", THIS_MODULE->name, link );
      err = -ENODEV; goto err;
   }
   if( priv->parent->type != ARPHRD_ETHER && priv->parent->type != ARPHRD_LOOPBACK ) {
      ERR( "%s: illegal net type", THIS_MODULE->name );
      err = -EINVAL; goto err;
   }
   memcpy( child->dev_addr, priv->parent->dev_addr, ETH_ALEN );
   memcpy( child->broadcast, priv->parent->broadcast, ETH_ALEN );
   if( ( err = dev_alloc_name( child, child->name ) ) ) {
      ERR( "%s: allocate name, error %i", THIS_MODULE->name, err );
      err = -EIO; goto err;
   }
   register_netdev( child );         // зарегистрировать новый интерфейс
   proto_parent.dev = priv->parent;
   dev_add_pack( &proto_parent );    // установить обработчик фреймов для родителя
   LOG( "module %s loaded", THIS_MODULE->name );
   LOG( "%s: create link %s", THIS_MODULE->name, child->name );
   return 0;
err:
   free_netdev( child );
   return err;
}

void __exit virt_exit( void ) {
   dev_remove_pack( &proto_parent ); // удалить обработчик фреймов
   unregister_netdev( child );
   free_netdev( child );
   LOG( "module %s unloaded", THIS_MODULE->name );
   LOG( "=============================================" );
}

module_init( init );
module_exit( virt_exit );

MODULE_AUTHOR( "Oleg Tsiliuric" );
MODULE_LICENSE( "GPL v2" );
MODULE_VERSION( "3.6" );
