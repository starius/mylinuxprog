#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

static int mode = 0; // выделение памяти: 0 - kmalloc(), 1 - __get_free_pages(), 2 - vmalloc()
module_param( mode, int, S_IRUGO );

char *mfun[] = { "kmalloc", "__get_free_pages", "vmalloc" };

static int __init init( void ) {
   static char *kbuf;
   static unsigned long order, size;
   if( mode < 0 || mode > 2 ) {
      printk( KERN_ERR "illegal mode value\n" );
      return -EINVAL;
   }
   for( size = PAGE_SIZE, order = 0; ; order++, size *= 2 ) {
      char msg[ 120 ];
      sprintf( msg, "order=%2ld, pages=%6ld, size=%9ld - %s ", order, size / PAGE_SIZE, size, mfun[ mode ] );
      switch( mode ) {
         case 0:
            kbuf = (char *)kmalloc( (size_t)size, GFP_KERNEL ); 
            break;
         case 1:
            kbuf = (char *)__get_free_pages( GFP_KERNEL, order );
            break;
         case 2:
            kbuf = (char *)vmalloc( size );
            break;
      }
      strcat( msg, kbuf ? "OK\n" : "failed\n" );
      printk( KERN_INFO "%s", msg );
      if( !kbuf ) break;
      switch( mode ) {
         case 0:
            kfree( kbuf );
            break;
         case 1:
            free_pages( (unsigned long)kbuf, order );
            break;
         case 2:
            vfree( kbuf );
            break;
      }
   }
   return -1;
}
module_init( init );

MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_DESCRIPTION( "memory allocation size test" );
MODULE_LICENSE( "GPL v2" );
