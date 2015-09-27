#include <linux/module.h>

extern int sys_open( int fd );

static int __init sys_init( void ) {
   void* Addr;
   Addr = (void*)sys_open;
   printk( KERN_INFO "sys_open address: %p\n", Addr );
   return -1;
}

module_init( sys_init );

