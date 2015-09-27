#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#define LOG(...) printk( KERN_INFO "+ "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR  "+ "__VA_ARGS__ )

static int mode = 0;   // выделение памяти: 0 - kmalloc(), 2 - vmalloc()
module_param( mode, uint, S_IRUGO );

static int debug = 0;  // уровень отладочной диагностики
module_param( debug, uint, S_IRUGO );

inline int ok( size_t lim ) { // тестирование размещения
   void* addr = 0 == mode ?
                kmalloc( lim, GFP_KERNEL | __GFP_NOWARN ) :
                __vmalloc( lim, GFP_KERNEL | __GFP_NOWARN, PAGE_KERNEL );
   if( addr ) {
      if( 0 == mode ) kfree( addr );
      else vfree( addr );
   }
   return addr != NULL;
}

static int __init init( void ) {
   size_t liml = 0, limh = PAGE_SIZE;
   if( mode > 1 ) {
      ERR( "illegal mode value\n" );
      return -EINVAL;
   }
   while( 1 ) {     // найти верхнюю границу
      int res = ok( limh );
      if( debug > 1 || ( !res && debug > 0 ) ) 
         LOG( "%s :\t%lu -> %s\n", 
              0 == mode ? "kmalloc" : "vmalloc",
              (long)limh,
              res ? "OK" : "failed" );
      if( !res ) break;
      limh *= 2;
   }
   liml = limh / 2;      
   while( 1 ) {     // сужение границ 
      size_t midle = limh / 2 + liml / 2;
      int res = ok( midle );
      if( debug > 1 || ( !res && debug > 0 ) ) 
         LOG( "%s :\t%lu...%lu -> %s\n", 
              0 == mode ? "kmalloc" : "vmalloc",
              (long)liml, (long)limh,
              res ? "OK" : "failed" );
      if( limh == midle || ( midle - liml ) < 2 ) { 
         if( res ) liml = midle;
         break;
      }    
      if( res ) liml = midle;
      else limh = midle;
   }
   LOG( "maximal allocation %lu (%lX) bytes\n", (long)liml, (long)liml );
   return -1;
}

static void __exit cleanup( void ) {}

module_init( init );
module_exit( cleanup );

MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_DESCRIPTION( "memory allocation size test" );
MODULE_LICENSE( "GPL v2" );
