#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>                                                                                            
#include <linux/jiffies.h>

#define LOG(...) printk( KERN_INFO "=== "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "=== "__VA_ARGS__ )

#define SHARED_IRQ 1
#define NAME_LEN   12
#define IRQ_ID     321

static int irq = SHARED_IRQ;           // контролируемая IRQ 
module_param( irq, int, 0 );
static unsigned int delay = 1;         // интервал индикации, сек.
module_param( delay, uint, 0 );

static atomic64_t counter = ATOMIC_LONG_INIT( 0 );
static volatile long unsigned int next; 

static irqreturn_t handler( int irq, void *id ) {
   atomic64_inc( &counter );
   if( time_after( jiffies, next ) ) { // прошло больше интервала
      LOG( "%ld => %ld\n", jiffies, (long)atomic64_read( &counter ) );
      next = jiffies + delay;
   }
   return IRQ_NONE;
}

int init_module( void ) {
   char dev[ NAME_LEN ];
   int ret;
   sprintf( dev, "handler-%02d", irq );
   if( ( ret = request_irq( irq, handler, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) 
      ERR( "can't install handler for IRQ=%d\n", irq );
   else {
      LOG( "module installed\n" );
      delay *= HZ;
      next = jiffies + delay;          // отметка старта
   }
   return ret;
}

void cleanup_module( void ) {
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
   LOG( "module removed: registered %lu events\n", (long)atomic64_read( &counter ) );
}

MODULE_LICENSE( "GPL v2" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
