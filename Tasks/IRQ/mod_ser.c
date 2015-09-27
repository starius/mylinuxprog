#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE( "GPL v2" );
#define SHARED_IRQ 1
#define MAX_SHARED 9
#define NAME_SUFFIX "serial_"
#define NAME_LEN   10
static int irq = SHARED_IRQ, num = 2; 
module_param( irq, int, 0 );
module_param( num, int, 0 );


static irqreturn_t handler( int irq, void *id ) {
   cycles_t cycles = get_cycles();
   printk( KERN_INFO "%010lld : irq=%d - handler #%ld\n", cycles, irq, (long)id );
   return IRQ_NONE;
}

static char dev[ MAX_SHARED ][ NAME_LEN ];

int init_module( void ) {
   long i;
   if( num > MAX_SHARED ) num = MAX_SHARED;
   for( i = 0; i < num; i++ ) {
      sprintf( dev[ i ], "serial_%02ld", i + 1 );
      if( request_irq( irq, handler, IRQF_SHARED, dev[ i ], (void*)( i + 1 ) ) ) return -1;
   }
   return 0;
}

void cleanup_module( void ) {
   long i;
   for( i = 0; i < num; i++ ) {
      synchronize_irq( irq );
      free_irq( irq, (void*)( i + 1 ) );
   }
}

