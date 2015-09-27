#include <linux/module.h>

static int __init hello_init( void ) {
//============== CONFIG_SMP=y
#if defined(CONFIG_SMP)
   printk( "CONFIG_SMP = %d\n", CONFIG_SMP );
#else
   printk( "CONFIG_SMP не определено\n" );
#endif
//============== CONFIG_64BIT is not set
#if defined(CONFIG_64BIT)
   printk( "CONFIG_64BIT = %d\n", CONFIG_64BIT );
#else
   printk( "CONFIG_64BIT не определено\n" );
#endif
//============== CONFIG_MICROCODE=m
#if defined(CONFIG_MICROCODE)
   printk( "CONFIG_MICROCODE = %d\n", CONFIG_MICROCODE );
#else
   printk( "CONFIG_MICROCODE не определено\n" );
#if defined(CONFIG_MICROCODE_MODULE)
   printk( "CONFIG_MICROCODE_MODULE = %d\n", CONFIG_MICROCODE_MODULE );
#else
   printk( "CONFIG_MICROCODE_MODULE не определено\n" );
#endif
#endif
//============== CONFIG_OUTPUT_FORMAT="elf32-i386"
#if defined(CONFIG_OUTPUT_FORMAT)
   printk( "CONFIG_OUTPUT_FORMAT = %s\n", CONFIG_OUTPUT_FORMAT );
#else
   printk( "CONFIG_OUTPUT_FORMAT не определено\n" );
#endif
   return -1;
}

module_init( hello_init );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
