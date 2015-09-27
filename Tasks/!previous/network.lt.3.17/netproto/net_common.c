#include <linux/module.h>
#include <linux/netdevice.h>

#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )
#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )

MODULE_AUTHOR( "Oleg Tsiliuric" );
MODULE_LICENSE( "GPL v2" );

static int __init my_init( void );
static void __exit my_exit( void );

module_init( my_init );
module_exit( my_exit );
