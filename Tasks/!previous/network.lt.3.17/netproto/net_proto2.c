#include "net_common.c"

static int debug = 0;
module_param( debug, int, 0 );

static char* link = NULL;
module_param( link, charp, 0 );

int test_pack_rcv_1( struct sk_buff *skb, struct net_device *dev,
                     struct packet_type *pt, struct net_device *odev ) {
   int s = atomic_read( &skb->users );  
   kfree_skb( skb );
   if( debug > 0 ) LOG( "function #1 - %p => users: %d->%d\n", skb, s, atomic_read( &skb->users ) );
   return skb->len;
};

int test_pack_rcv_2( struct sk_buff *skb, struct net_device *dev,
                     struct packet_type *pt, struct net_device *odev ) {
   int s = atomic_read( &skb->users );  
   kfree_skb( skb );
   if( debug > 0 ) LOG( "function #2 - %p => users: %d->%d\n", skb, s, atomic_read( &skb->users ) );
   return skb->len;
};

static struct packet_type 
test_proto1 = {
   __constant_htons( ETH_P_IP ),
   NULL,
   test_pack_rcv_1,
   (void*)1,
   NULL
},
test_proto2 = {
   __constant_htons( ETH_P_IP ),
   NULL,
   test_pack_rcv_2,
   (void*)1,
   NULL
};

static int __init my_init( void ) {
   if( link != NULL ) {
      struct net_device *dev = __dev_get_by_name( &init_net, link );
      if( NULL == dev ) { 
         ERR( "%s: illegal link", link );
         return -EINVAL; 
      }
      test_proto1.dev = test_proto2.dev = dev;
   }
   dev_add_pack( &test_proto1 );
   dev_add_pack( &test_proto2 );
   if( NULL == test_proto1.dev ) LOG( "module %s loaded for all links\n", THIS_MODULE->name );
   else LOG( "module %s loaded for link %s\n", THIS_MODULE->name, link );
   return 0; 
}

static void __exit my_exit( void ) {
   if( test_proto2.dev != NULL ) dev_put( test_proto2.dev );
   if( test_proto1.dev != NULL ) dev_put( test_proto1.dev );
   dev_remove_pack( &test_proto2 );
   dev_remove_pack( &test_proto1 );
   LOG( "module %s unloaded\n", THIS_MODULE->name );
}


/* struct packet_type {
   __be16 type;            // This is really htons(ether_type). 
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
