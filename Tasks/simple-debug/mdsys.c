#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/version.h>

#define SHARED_IRQ 16                // my eth0 interrupt
static int irq = SHARED_IRQ;
module_param( irq, int, S_IRUGO );   // may be change

static unsigned int irq_counter = 0;

static irqreturn_t mdsys_interrupt( int irq, void *dev_id ) {
   irq_counter++;
   return IRQ_NONE;
}

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
static int my_dev_id;

int __init init( void ) {
   int res = 0;
   mds_class = class_create( THIS_MODULE, "mds-class" );
   if( IS_ERR( mds_class ) ) printk( KERN_ERR "bad class create\n" );
   res = class_create_file( mds_class, &class_attr_mds );
   if( res != 0 ) printk( KERN_ERR "bad class create file\n" );
   if( request_irq( irq, mdsys_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id ) )
      res = -1;
   return res;
}

void cleanup( void ) {
   synchronize_irq( irq );
   free_irq( irq, &my_dev_id );
   class_remove_file( mds_class, &class_attr_mds );
   class_destroy( mds_class );
   return;
}

module_init( init );
module_exit( cleanup );

MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_DESCRIPTION( "module in debug" );
MODULE_LICENSE( "GPL v2" );
