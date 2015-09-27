#include <linux/module.h>
#include <linux/sched.h> 
#include <linux/delay.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )

static void wq_function( struct work_struct *work ) { // рабочая функция
   LOG( "%d: start time %lX\n", current->pid, jiffies );
   msleep( 1000 );                                    // пауза 1с. 
   LOG( "%d: finish time %lX\n", current->pid, jiffies );
   return;
}

int test_thread( void ) { 
   int ret;
   struct work_struct work;
   unsigned long j = jiffies;
   struct workqueue_struct *wq = create_workqueue( "queue" );
   if( !wq ) return -EBADRQC;
   INIT_WORK( &work, wq_function );
   ret = queue_work( wq, &work );
   flush_workqueue( wq );
   destroy_workqueue( wq );
   j = jiffies - j;
   LOG( "%d: evaluation time was %ld millisec.\n", current->pid, 1000 * j / HZ );
   if( !ret ) return -EPERM;
   return -1;
}

module_init( test_thread ); 

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
