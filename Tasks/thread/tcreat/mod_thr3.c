#include <linux/module.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/jiffies.h> 

#include "../prefix.c"

static int N = 2;            // N - число потоков
module_param( N, int, 0 );

static int thread_fun1( void* data ) { 
   int N = (int)data - 1;
   struct task_struct *t1 = NULL;
   printk( "%s is parent [%05d]\n", st( N ), current->parent->pid ); 
   if( N > 0 )
      t1 = kthread_run( thread_fun1, (void*)N, "my_thread_%d", N );
   while( !kthread_should_stop() ) {
      // выполняемая работа потоковой функции 
      msleep( 100 ); 
   }
   printk( "%s find signal!\n", st( N ) );
   if( t1 != NULL ) kthread_stop( t1 );   
   printk( "%s is completed\n", st( N ) );
   return 0;
}

static int test_thread( void ) { 
   struct task_struct *t1;
   printk( "%smain process [%d] is running\n", sj(), current->pid ); 
   t1 = kthread_run( thread_fun1, (void*)N, "my_thread_%d", N );
   msleep( 10000 ); 
   kthread_stop( t1 );   
   printk( "%smain process [%d] is completed\n", sj(), current->pid );
   return -1; 
} 

module_init( test_thread ); 
MODULE_LICENSE( "GPL" );
