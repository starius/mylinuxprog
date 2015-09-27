#include <linux/module.h>
#include <linux/sched.h> 
#include <linux/delay.h>
#include <linux/interrupt.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )

static void tasklet_fn( unsigned long data ) { 
   unsigned long j;
   LOG( "%d: start time %lX\n", current->pid, jiffies );
   j = jiffies; 
   while( jiffies - j < HZ );             // активная пауза 1с. 
   LOG( "%d: finish time %lX\n", current->pid, jiffies );
   return;
}

int test_thread( void ) { 
   unsigned long j = jiffies;
   DECLARE_TASKLET( tasklet, tasklet_fn, (unsigned long)NULL );
   tasklet_schedule( &tasklet );          // запланировать на выполнение  
   tasklet_kill( &tasklet );              // остановить тасклет перед завершением
   j = jiffies - j;
   LOG( "%d: evaluation time was %ld millisec.\n", current->pid, 1000 * j / HZ );
   return -1;
}

module_init( test_thread ); 

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

