#include <linux/module.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/slab.h>

static ktime_t tout;

static struct kt_data {
   struct hrtimer timer;
   ktime_t        period;
   int            numb;
} *data;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static int ktfun( struct hrtimer *var ) {
#else        
static enum hrtimer_restart ktfun( struct hrtimer *var ) {
#endif
   ktime_t now = var->base->get_time();    
     // текущее время в типе ktime_t
   printk( KERN_INFO "timer run #%d at jiffies=%ld\n", data->numb, jiffies );
   hrtimer_forward( var, now, tout );
   return data->numb-- > 0? HRTIMER_RESTART : HRTIMER_NORESTART;
}

int __init hr_init( void ) {
   enum hrtimer_mode mode;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
   mode = HRTIMER_REL;
#else
   mode = HRTIMER_MODE_REL;
#endif
   tout = ktime_set( 1, 0 );       /* 1 sec. + 0 nsec. */
   data = kmalloc( sizeof(*data), GFP_KERNEL );
   data->period = tout;
   hrtimer_init( &data->timer, CLOCK_REALTIME, mode );
   data->timer.function = ktfun;
   data->numb = 3;
   printk( KERN_INFO "timer start at jiffies=%ld\n", jiffies );   
   hrtimer_start( &data->timer, data->period, mode );
   return 0;
}

void hr_cleanup( void ) {
   hrtimer_cancel( &data->timer );
   kfree( data );
   return;
}

module_init( hr_init );
module_exit( hr_cleanup );
MODULE_LICENSE( "GPL" );
