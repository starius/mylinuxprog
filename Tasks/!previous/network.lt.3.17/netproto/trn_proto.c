#include <net/protocol.h>
#include "net_common.c"

int test_proto_rcv( struct sk_buff *skb ) {
   printk( KERN_INFO "Packet received with length: %u\n", skb->len );
   return skb->len;
};

/* This is used to register protocols. 
struct net_protocol {
   int  (*handler)( struct sk_buff *skb );
   void (*err_handler)( struct sk_buff *skb, u32 info );
   int  (*gso_send_check)( struct sk_buff *skb );
   struct sk_buff *(*gso_segment)( struct sk_buff *skb,
                                   int features);
   struct sk_buff **(*gro_receive)( struct sk_buff **head,
                                    struct sk_buff *skb );
   int (*gro_complete)( struct sk_buff *skb );
   unsigned int no_policy:1,
                netns_ok:1;
}; */
static struct net_protocol test_proto = {
   .handler = test_proto_rcv,
   .err_handler = 0,
   .no_policy = 0,
};

#define PROTO IPPROTO_ICMP
//#define PROTO IPPROTO_TCP
//#define PROTO IPPROTO_RAW
// #define PROTO IPPROTO_UDP

static int __init my_init( void ) {
   int ret;
   if( ( ret = inet_add_protocol( &test_proto, PROTO ) ) < 0 ) {
      printk( KERN_INFO "proto init: can't add protocol\n");
      return ret;
   };
   printk( KERN_INFO "proto module loaded\n" );
   return 0; 
}

static void __exit my_exit( void ) {
   inet_del_protocol( &test_proto, PROTO );
   printk( KERN_INFO "proto module unloaded\n" );
}

