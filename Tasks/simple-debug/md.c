#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init( void ) {
   extern int sys_close( int fd );
   void* Addr;
   Addr = (void*)sys_close;
   printk( KERN_INFO "sys_close address: %p\n", Addr );
   return -1;
}

module_init( hello_init );
