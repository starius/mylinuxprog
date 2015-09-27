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
static struct net_device_stats stats;
static u32 child_ip;

struct priv {
   struct net_device *parent;
};

static char* strIP( u32 addr ) {     // диагностика IP в точечной нотации
   static char saddr[ MAX_ADDR_LEN ];
   sprintf( saddr, "%d.%d.%d.%d",
            ( addr ) & 0xFF, ( addr >> 8 ) & 0xFF,
            ( addr >> 16 ) & 0xFF, ( addr >> 24 ) & 0xFF
          );
   return saddr;
}

static char* strAR_IP( unsigned char addr[ 4 ] ) {
   static char saddr[ MAX_ADDR_LEN ];
   sprintf( saddr, "%d.%0d.%d.%d",
            addr[ 0 ], addr[ 1 ], addr[ 2 ], addr[ 3 ] );
   return saddr;
}

//<linux/if_arp.h> :
struct arp_eth_body {
   unsigned char  ar_sha[ ETH_ALEN ];     // sender hardware address      
   unsigned char  ar_sip[ 4 ];            // sender IP address            
   unsigned char  ar_tha[ ETH_ALEN ];     // target hardware address      
   unsigned char  ar_tip[ 4 ];            // target IP address            
};

static rx_handler_result_t handle_frame( struct sk_buff **pskb ) {
   struct sk_buff *skb = *pskb;
   if( skb->protocol == htons( ETH_P_IP ) ) {
      struct iphdr *ip = ip_hdr( skb );
      char *addr = strIP( ip->daddr );
      DBG( "rx: IP4 to IP=%s", addr );
      if( ip->daddr != child_ip )
         return RX_HANDLER_PASS; 
   }
   else if( skb->protocol == htons( ETH_P_ARP ) ) {
      struct arphdr *arp = arp_hdr( skb );
      struct arp_eth_body *body = (void*)arp + sizeof( struct arphdr ); 
      int i, ip = child_ip;
      DBG( "rx: ARP for %s", strAR_IP( body->ar_tip ) );
      for( i = 0; i < sizeof( body->ar_tip ); i++ ) {
         if( ( ip & 0xFF ) != body->ar_tip[ i ] ) break;
         ip = ip >> 8;
      }
      if( i < sizeof( body->ar_tip ) )
         return RX_HANDLER_PASS; 
   }
   else           // не ARP и не IP4
      return RX_HANDLER_PASS; 
   stats.rx_packets++;
   stats.rx_bytes += skb->len;
   DBG( "rx: injecting frame from %s to %s", skb->dev->name, child->name );
   skb->dev = child;
   return RX_HANDLER_ANOTHER;
}

static netdev_tx_t start_xmit( struct sk_buff *skb, struct net_device *dev ) {
   struct priv *priv = netdev_priv( dev );
   stats.tx_packets++;
   stats.tx_bytes += skb->len;
   if( priv->parent ) {
      skb->dev = priv->parent;
      skb->priority = 1;
      dev_queue_xmit( skb );
      LOG( "tx: injecting frame from %s to %s", dev->name, skb->dev->name );
      return 0;
   }
   return NETDEV_TX_OK;
}

static int open( struct net_device *dev ) {
   struct in_device *in_dev = dev->ip_ptr;
   struct in_ifaddr *ifa = in_dev->ifa_list;      /* IP ifaddr chain */
   char sdebg[ 40 ] = "";
   LOG( "%s: device opened", dev->name );
   child_ip = ifa->ifa_address;
   sprintf( sdebg, "%s:", strIP( ifa->ifa_address ) );
   strcat( sdebg, strIP( ifa->ifa_mask ) );
   DBG( "%s: %s", dev->name, sdebg );
   netif_start_queue( dev );
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

static struct net_device_ops crypto_net_device_ops = {
   .ndo_open = open,
   .ndo_stop = stop,
   .ndo_get_stats = get_stats,
   .ndo_start_xmit = start_xmit,
};

static void setup( struct net_device *dev ) {
   int j;
   ether_setup( dev );
   memset( netdev_priv(dev), 0, sizeof( struct priv ) );
   dev->netdev_ops = &crypto_net_device_ops;
   for( j = 0; j < ETH_ALEN; ++j ) // fill in the MAC address with a phoney 
      dev->dev_addr[ j ] = (char)j;
}

int __init init( void ) {
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
   priv->parent = __dev_get_by_name( &init_net, link ); // parent interface  
   if( !priv->parent ) {
      ERR( "%s: no such net: %s", THIS_MODULE->name, link );
      err = -ENODEV; goto err;
   }
   if( priv->parent->type != ARPHRD_ETHER && priv->parent->type != ARPHRD_LOOPBACK ) {
      ERR( "%s: illegal net type", THIS_MODULE->name );
      err = -EINVAL; goto err;
   }
   /* also, and clone its IP, MAC and other information */
   memcpy( child->dev_addr, priv->parent->dev_addr, ETH_ALEN );
   memcpy( child->broadcast, priv->parent->broadcast, ETH_ALEN );
   if( ( err = dev_alloc_name( child, child->name ) ) ) {
      ERR( "%s: allocate name, error %i", THIS_MODULE->name, err );
      err = -EIO; goto err;
   }
   register_netdev( child );
   rtnl_lock();
   netdev_rx_handler_register( priv->parent, &handle_frame, NULL );
   rtnl_unlock();
   LOG( "module %s loaded", THIS_MODULE->name );
   LOG( "%s: create link %s", THIS_MODULE->name, child->name );
   LOG( "%s: registered rx handler for %s", THIS_MODULE->name, priv->parent->name );
   return 0;
err:
   free_netdev( child );
   return err;
}

void __exit virt_exit( void ) {
   struct priv *priv = netdev_priv( child );
   rtnl_lock();
   netdev_rx_handler_unregister( priv->parent );
   rtnl_unlock();
   LOG( "%s: unregister rx handler for %s\n", THIS_MODULE->name, priv->parent->name );
   unregister_netdev( child );
   free_netdev( child );
   LOG( "module %s unloaded", THIS_MODULE->name );
}

module_init( init );
module_exit( virt_exit );

MODULE_AUTHOR( "Oleg Tsiliuric" );
MODULE_AUTHOR( "Nikita Dorokhin" );
MODULE_LICENSE( "GPL v2" );
MODULE_VERSION( "3.4" );

