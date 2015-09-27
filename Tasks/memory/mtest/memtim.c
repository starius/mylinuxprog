#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/msr.h>
#include <linux/sched.h>

static long size = 1000;  
module_param( size, long, 0 );

#define CYCLES 1024                       // число циклов накопления

static int __init init( void ) {
   int i;
   unsigned long order = 1, psize;
   unsigned long long calibr = 0;
   const char *mfun[] = { "kmalloc", "__get_free_pages", "vmalloc" };
   for( psize = PAGE_SIZE; psize < size; order++, psize *= 2 );
   printk( KERN_INFO "size = %ld order = %ld(%ld)\n", size, order, psize );
   for( i = 0; i < CYCLES; i++ ) {       // калибровка времени выполнения rdtscll()
      unsigned long long t1, t2;
      schedule();                        // обеспечивает лучшую повторяемость
      rdtscll( t1 );
      rdtscll( t2 );
      calibr += ( t2 - t1 );
   }
   calibr = calibr / CYCLES;
   for( i = 0; i < sizeof( mfun ) / sizeof( mfun[ 0 ] ); i++ ) {
      char *kbuf;
      char msg[ 120 ];
      int j;
      unsigned long long suma = 0;
      sprintf( msg, "proc. cycles for allocate %s : ", mfun[ i ] );
      for( j = 0; j < CYCLES; j++ ) {    // циклы накопления измерений
         unsigned long long t1, t2;
         schedule();                     // обеспечивает лучшую повторяемость
         rdtscll( t1 );
         switch( i ) {
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
         if( !kbuf ) break;
         rdtscll( t2 );
         suma += ( t2 - t1 - calibr );
         switch( i ) {
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
      if( kbuf )
         sprintf( ( msg + strlen( msg ) ), "%lld", ( suma / CYCLES ) );
      else
         strcat( msg, "failed" );
      printk( KERN_INFO "%s\n", msg );
   }
   return -1;
}
module_init( init );

MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_DESCRIPTION( "memory allocation speed test" );
MODULE_LICENSE( "GPL v2" );
