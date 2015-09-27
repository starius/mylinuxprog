#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/pci.h>  // здесь описание class_create_file()

#define LOG(...) printk( KERN_INFO "=== "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "=== "__VA_ARGS__ )

#define SHARED_IRQ 1
#define NAME_LEN   12
#define IRQ_ID     321

static int irq = SHARED_IRQ;           // контролируемая IRQ 
module_param( irq, int, 0 );

static atomic64_t counter = ATOMIC_LONG_INIT( 0 );
static volatile long unsigned int next; 

static irqreturn_t handler( int irq, void *id ) {
   atomic64_inc( &counter );
   return IRQ_NONE;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t node_read( struct class *class, struct class_attribute *attr, char *buf ) {
#else
static ssize_t node_read( struct class *class, char *buf ) {
#endif
   char buf_msg[ 30 ];
   sprintf( buf_msg, "%lu\n", (long)atomic64_read( &counter ) ); 
   strcpy( buf, buf_msg );
   printk( "read %ld\n", (long)strlen( buf ) );
   return strlen( buf );
}

#define NAME_DIR  "irq_counter" 
//#define NAME_NODE counter

CLASS_ATTR( counter, S_IRUGO, &node_read, NULL );

static struct class *irq_class;

int init_module( void ) {
   char dev[ NAME_LEN ];
   int ret;
   sprintf( dev, "handler-%02d", irq );
   if( ( ret = request_irq( irq, handler, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) {
      ERR( "can't install handler for IRQ=%d\n", irq );
      goto err_irq;
   }      
   irq_class = class_create( THIS_MODULE, NAME_DIR );
   if( IS_ERR( irq_class ) ) {
      ERR( "can't create directory /sys/class/%s\n", NAME_DIR );
      goto err_dir;
   }
   ret = class_create_file( irq_class, &class_attr_counter );
/* <linux/device.h>
   extern int __must_check class_create_file(struct class *class, const struct class_attribute *attr); */
   if( ret != 0 ) {
      ERR( "can't create node /sys/class/%s/counter\n", NAME_DIR );
      goto err_node;
   }
   LOG( "register node /sys/class/%s/counter\n", NAME_DIR );
   return 0;
err_node:
   class_destroy( irq_class );
err_dir:
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
err_irq:
   return ret;
}

void cleanup_module( void ) {
/* <linux/device.h>
extern void class_remove_file(struct class *class, const struct class_attribute *attr); */
   class_remove_file( irq_class, &class_attr_counter );
   class_destroy( irq_class );
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
   LOG( "module removed: registered %lu events\n", (long)atomic64_read( &counter ) );
}

MODULE_LICENSE( "GPL v2" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
