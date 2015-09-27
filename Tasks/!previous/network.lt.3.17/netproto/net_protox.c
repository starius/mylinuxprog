#include "net_common.c"

int test_pack_rcv( struct sk_buff *skb, struct net_device *dev,
                   struct packet_type *pt, struct net_device *odev ) {
   LOG( "packet received with length: %u\n", skb->len );
   kfree_skb( skb );
   return skb->len;
};

#define TEST_PROTO_ID 0x1234
static struct packet_type test_proto = {
//   __constant_htons( TEST_PROTO_ID ),
   __constant_htons( ETH_P_IP ),
   NULL,
   test_pack_rcv,
   (void*)1,
   NULL
};

static int my_open( struct net_device *dev ) {
   LOG( "my_open(%s)\n", dev->name );
   /* start up the transmission queue */
   netif_start_queue( dev );
   return 0;
}

static int my_close( struct net_device *dev ) {
   LOG( "my_close(%s)\n", dev->name );
   /* shutdown the transmission queue */
   netif_stop_queue( dev );
   return 0;
}

static int stub_start_xmit( struct sk_buff *skb, struct net_device *dev ) {
   dev_kfree_skb( skb );
   return 0;
}

static struct net_device_ops ndo = {
   .ndo_open = my_open,
   .ndo_stop = my_close,
   .ndo_start_xmit = stub_start_xmit,
};

static void my_setup( struct net_device *dev ) {
   int j;
   LOG( "setup interface %s\n", dev->name );
   for( j = 0; j < ETH_ALEN; ++j )
      dev->dev_addr[ j ] = (char)j;    // Fill the MAC address
   ether_setup( dev );
   dev->netdev_ops = &ndo;
}

static struct net_device *dev;

static int __init my_init( void ) {
   dev = alloc_netdev( 0, "xxx%d", my_setup );
   if( register_netdev( dev ) ) {
      ERR( "failed to register\n" );
      free_netdev( dev );
      return -1;
   }
   test_proto.dev = dev;
   dev_add_pack( &test_proto );
   LOG( "module loaded\n" );
   return 0; 
}

static void __exit my_exit( void ) {
   dev_remove_pack( &test_proto );
   unregister_netdev( dev );
   free_netdev( dev );
   LOG( "module unloaded\n" );
}

/* struct packet_type {
   __be16 type;            // This is really htons(ether_type)
   struct net_device *dev; // NULL is wildcarded here
   (*func) ( struct sk_buff *, struct net_device *, 
             struct packet_type *, struct net_device * );
   struct sk_buff *(*gso_segment)( struct sk_buff *skb, int features );
   int (*gso_send_check)( struct sk_buff *skb );
   struct sk_buff **(*gro_receive)( struct sk_buff **head, struct sk_buff *skb );
   int (*gro_complete)( struct sk_buff *skb );
   void *af_packet_priv;
   struct list_head list;
}; */
