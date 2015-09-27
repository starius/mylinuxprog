#include <linux/module.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )

// current - указатель на дескриптор текущей задачи:
static int threadfn( void *data ) { 
   struct completion *finished = (struct completion*)data; 
   LOG( "%d: start time %lX\n", current->pid, jiffies ); 
   msleep( 1000 );                        // пауза 1с. 
   complete( finished );                  // отмечаем факт выполнения условия
   LOG( "%d: finish time %lX\n", current->pid, jiffies ); 
   return 0; 
} 

int test_thread( void ) { 
   DECLARE_COMPLETION( finished );
   unsigned long j = jiffies;
   struct task_struct *t = kthread_run( threadfn, &finished, "new_thread" );
   if( NULL == t ) return -EINVAL;
   wait_for_completion( &finished );      // ожидаем выполнения условия 
   j = jiffies - j;      
   LOG( "%d: evaluation time was %ld millisec.\n", current->pid, 1000 * j / HZ );
   return -1; 
} 

module_init( test_thread ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

