#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/delay.h> 

static int thread( void * data ) { 
   struct completion *finished = (struct completion*)data; 
   struct task_struct *curr = current;                       /* current - указатель на дескриптор текущей задачи */ 
   printk( "child process [%d] is running\n", curr->pid );
   msleep( 10000 );                                          /* Пауза 10 с. */
   printk( "child process [%d] is completed\n", curr->pid ); 
   complete( finished );                                     /* Отмечаем факт выполнения условия. */
   return 0; 
} 

int test_thread( void ) { 
   pid_t pid; 
   DECLARE_COMPLETION( finished ); 
   struct task_struct *curr = current;
   printk( "main process [%d] is running\n", curr->pid ); 
   pid = kernel_thread( thread, &finished, CLONE_FS ); /* Запускаем новый поток */ 
   printk( "create new kernel thread [%d]\n", pid ); 
   msleep( 5000 );                                           /* Пауза 5 с. */
   wait_for_completion( &finished );                         /* Ожидаем выполнения условия */
   printk( "main process [%d] is completed\n", curr->pid ); 
   return -1; 
} 

module_init( test_thread ); 
MODULE_LICENSE( "GPL" );
