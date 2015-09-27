#include "mdsys2.h"

#define SHARED_IRQ 16                // my eth0 interrupt
static int irq = SHARED_IRQ;
module_param( irq, int, S_IRUGO );   // may be change

unsigned int irq_counter = 0;
EXPORT_SYMBOL( irq_counter );

static irqreturn_t mdsys_interrupt( int irq, void *dev_id ) {
   irq_counter++;
   return IRQ_NONE;
}

static int my_dev_id;

int __init init( void ) {
   if( request_irq( irq, mdsys_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id ) )
      return -1;
   else 
      return 0;
}

void cleanup( void ) {
   synchronize_irq( irq );
   free_irq( irq, &my_dev_id );
   return;
}

