#include "../dev.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/init.h>

static int major = 0;
module_param( major, int, S_IRUGO );

MODULE_AUTHOR( "Timur G. Baymatov" );

static const struct file_operations dev_fops = {
   .owner = THIS_MODULE,
   .read  = dev_read,
};

#define DEVICE_FIRST  0
#define DEVICE_COUNT  3
#define MODNAME "my_dyndev_mod"

static struct cdev hcdev;
static struct class *devclass;

static int __init dev_init( void ) {
   int ret, i;
   dev_t dev;
   if( major != 0 ) {
      dev = MKDEV( major, DEVICE_FIRST );
      ret = register_chrdev_region( dev, DEVICE_COUNT, MODNAME );
   }
   else {
      ret = alloc_chrdev_region( &dev, DEVICE_FIRST, DEVICE_COUNT, MODNAME );
      major = MAJOR( dev );  // не забыть зафиксировать!
   }
   if( ret < 0 ) {
      printk( KERN_ERR "=== Can not register char device region\n" );
      goto err;
   }
   cdev_init( &hcdev, &dev_fops );
   hcdev.owner = THIS_MODULE;
   ret = cdev_add( &hcdev, dev, DEVICE_COUNT );
   if( ret < 0 ) {
      unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
      printk( KERN_ERR "=== Can not add char device\n" );
      goto err;
   }
   devclass = class_create( THIS_MODULE, "dyn_class" );
#define DEVNAME "dyn"
   for( i = 0; i < DEVICE_COUNT; i++ ) {
      char name[ 10 ];
      dev = MKDEV( major, DEVICE_FIRST + i );
      sprintf( name, "%s_%d", DEVNAME, i );
      device_create( devclass, NULL, dev, "%s", name );
   }
   printk( KERN_INFO "======== module installed %d:[%d-%d] ===========\n",
           MAJOR( dev ), DEVICE_FIRST, MINOR( dev ) ); 
err:
   return ret;
}

static void __exit dev_exit( void ) {
   dev_t dev;
   int i;
   for( i = 0; i < DEVICE_COUNT; i++ ) {
      dev = MKDEV( major, DEVICE_FIRST + i );
      device_destroy( devclass, dev );
   }
   class_destroy( devclass );
   cdev_del( &hcdev );
   unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
   printk( KERN_INFO "=============== module removed ==================\n" );
}
