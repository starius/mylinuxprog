#include "mdsys2.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t show( struct class *class, struct class_attribute *attr, char *buf ) {
#else
static ssize_t show( struct class *class, char *buf ) {
#endif
   sprintf( buf, "%d\n", irq_counter );
   return strlen( buf );
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {
#else
static ssize_t store( struct class *class, const char *buf, size_t count ) {
#endif
   int i, res = 0;
   const char dig[] = "0123456789";
   for( i = 0; i < count; i++ ) {
      char *p = strchr( dig, (int)buf[ i ] );
      if( NULL == p ) break;
      res = res * 10 + ( p - dig );
   }
   irq_counter = res;
   return count;
}

CLASS_ATTR( mds, 0666, &show, &store ); // => struct class_attribute class_attr_mds

static struct class *mds_class;

int __init init( void ) {
   int res = 0;
   mds_class = class_create( THIS_MODULE, "mds-class" );
   if( IS_ERR( mds_class ) ) printk( KERN_ERR "bad class create\n" );
   res = class_create_file( mds_class, &class_attr_mds );
   if( res != 0 ) printk( KERN_ERR "bad class create file\n" );
   return res;
}

void cleanup( void ) {
   class_remove_file( mds_class, &class_attr_mds );
   class_destroy( mds_class );
   return;
}
