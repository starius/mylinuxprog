#include "net_common.c"

int test_pack_rcv( struct sk_buff *skb, struct net_device *dev,
                   struct packet_type *pt, struct net_device *odev ) {
//   printk( KERN_INFO "packet received with length: %u\n", skb->len );
   kfree_skb( skb );
   return skb->len;
};

#define TEST_PROTO_ID 0x1234
static struct packet_type test_proto = {
   __constant_htons( ETH_P_ALL ),  // may be: __constant_htons( TEST_PROTO_ID ),
   NULL,
   test_pack_rcv,
   (void*)1,
   NULL
};

static int __init my_init( void ) {
   dev_add_pack( &test_proto );
   LOG( "module %s loaded\n", THIS_MODULE->name );
   return 0; 
}

static void __exit my_exit( void ) {
   dev_remove_pack( &test_proto );
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
