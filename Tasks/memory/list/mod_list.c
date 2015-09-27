#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_LICENSE( "GPL" );

static int size = 5;
module_param( size, int, S_IRUGO | S_IWUSR );

struct data {
    int n;
    struct list_head list;
};

void test_lists( void ) {
   struct list_head *iter, *iter_safe;
   struct data *item;
   int i;
   LIST_HEAD( list );
   for( i = 0; i < size; i++ ) {
      item = kmalloc( sizeof(*item), GFP_KERNEL );
      if( !item ) goto out;
      item->n = i;
      list_add( &(item->list), &list );
   }
   list_for_each( iter, &list ) {
      item = list_entry( iter, struct data, list );
      printk( KERN_INFO "[LIST] %d\n", item->n );
   }
out:
   list_for_each_safe( iter, iter_safe, &list ) {
      item = list_entry( iter, struct data, list );
      list_del( iter );
      kfree( item );
   }
}

static int __init mod_init( void ) {
   test_lists();
   return -1;
}
         
module_init( mod_init );
         
