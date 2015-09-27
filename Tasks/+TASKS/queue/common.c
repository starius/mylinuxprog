#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_VERSION( "6.4" );

static int minor = 0;
module_param( minor, int, S_IRUGO );

static bool debug = false;  // логический парамер
module_param( debug, bool, S_IRUGO );
#define LOG(...) if( debug ) printk( KERN_INFO "=== "__VA_ARGS__ )

static ssize_t queue_read( struct file*, char*, size_t, loff_t* );
static ssize_t queue_write( struct file*, const char*, size_t, loff_t* );

static const struct file_operations queue_fops = {
   .owner = THIS_MODULE,
   .read  = queue_read,     // чтение из /dev/queue 
   .write = queue_write     // запись в /dev/queue
};

static struct miscdevice queue_dev = {
   MISC_DYNAMIC_MINOR,      // автоматически выбираемое
   "queue",
   &queue_fops,
   .mode = 0666             // флаги устройства
};

static int __init dev_init( void );
static void __exit dev_exit( void );

module_init( dev_init );
module_exit( dev_exit );
