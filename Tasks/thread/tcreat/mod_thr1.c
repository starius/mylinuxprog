#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/delay.h> 

static int param = 3;
module_param( param, int, 0 );

static int thread( void * data ) { 
   printk( KERN_INFO "thread: child process [%d] is running\n", current->pid ); 
   ssleep( param );                                   /* Пауза 3 с. или как параметр укажет... */ 
   printk( KERN_INFO "thread: child process [%d] is completed\n", current->pid ); 
   return 0; 
} 

int test_thread( void ) {
   pid_t pid; 
   printk( KERN_INFO "thread: main process [%d] is running\n", current->pid ); 
   pid = kernel_thread( thread, NULL, CLONE_FS );     /* Запускаем новый поток */ 
   ssleep( 5 );                                       /* Пауза 5 с. */ 
   printk( KERN_INFO "thread: main process [%d] is completed\n", current->pid ); 
   return -1; 
} 

module_init( test_thread ); 
MODULE_LICENSE( "GPL" );
