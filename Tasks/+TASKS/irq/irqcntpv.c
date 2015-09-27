#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>

#include <linux/printk.h>

#define TITL THIS_MODULE->name
#define LOG(...) printk( KERN_INFO __VA_ARGS__ )
#define ERR(...) printk( KERN_ERR  __VA_ARGS__ )

#define SHARED_IRQ 1
#define NAME_LEN   12
#define IRQ_ID     321

static int irq = SHARED_IRQ;                  // контролируемая IRQ 
module_param( irq, int, 0 );
#define FIXLEN 2
static char smode[ FIXLEN ] = "i";            // режим обработчика прерывания
module_param_string( mode, smode, sizeof( smode ), 0 );

static atomic64_t counter = ATOMIC_LONG_INIT( 0 );
static volatile long unsigned int next; 

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
static irqreturn_t handler_irq( int irq, void *id ) {
   atomic64_inc( &counter );
   return IRQ_NONE;
}

static void irq_tsk( unsigned long data ) {
   atomic64_inc( &counter );
}

DECLARE_TASKLET( cnt_tasklet, irq_tsk, 0 );

static irqreturn_t handler_tsk( int irq, void *id ) {
   tasklet_schedule( &cnt_tasklet ); 
   return IRQ_NONE;
}

//static void irq_wrk( void *data ) {
//typedef void (*work_func_t)(struct work_struct *work);
static void irq_wrk( struct work_struct *data ) {
   atomic64_inc( &counter );
}

static struct workqueue_struct *wq;
DECLARE_WORK( cnt_work, irq_wrk ); //, NULL ); 

static irqreturn_t handler_wrk( int irq, void *id ) {
   queue_work( wq, &cnt_work ); 
   return IRQ_NONE;
}

static struct proc_dir_entry *own_proc_dir;
#endif

static ssize_t node_read( struct file *file, char *buf,
                          size_t count, loff_t *ppos ) {
   static unsigned int read_count = 1;
   if( ( ++read_count & 1 ) != 0 ) {
      *ppos = 0;
      return 0;                        // EOF
   }   
   else {
      char buf_msg[ 30 ];
      sprintf( buf_msg, "%lu\n", (long)atomic64_read( &counter ) ); 
      if( copy_to_user( buf, buf_msg, strlen( buf_msg ) ) ) return -EFAULT;
      return ( *ppos = strlen( buf_msg ) );
   }
}

static const struct file_operations node_fops = {
   .owner = THIS_MODULE,
   .read  = node_read,                 // чтение из /proc/irq_counter/counter
};

#define NAME_DIR  "irq_counter" 
#define NAME_NODE "counter"

int init_module( void ) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
   ERR( "%s: it's work only for kernel LE 3.9\n", TITL );
   return -ECANCELED;
#else
   char dev[ NAME_LEN ];
   int ret;
   struct proc_dir_entry *own_proc_node;
   LOG( "%s: interrupt mode = %c\n", TITL, smode[ 0 ] );
   sprintf( dev, "handler-%02d", irq );
   switch( smode[ 0 ] ) {
      case 'i' :                  // обработка в верхней половине
         if( ( ret = request_irq( irq, handler_irq, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) {
            ERR( "%s: can't install handler for IRQ=%d\n", TITL, irq );
            goto err_irq;
         }      
         break;
      case 't' :                  // обработка в тасклете
         if( ( ret = request_irq( irq, handler_tsk, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) {
            ERR( "%s: can't install handler for IRQ=%d\n", TITL, irq );
            goto err_irq;
         }      
         break;
      case 'w' :                  // обработка в очереди отложенных работ
         if( ( wq = create_workqueue( "new_queue" ) ) ) {
            if( ( ret = request_irq( irq, handler_wrk, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) {
               ERR( "%s: can't install handler for IRQ=%d\n", TITL, irq );
               destroy_workqueue( wq );
               goto err_irq;
            }      
         }
         else {
            ret = -EBADRQC;
            goto err_irq;
         }
         break;        
      default  :
         ERR( "%s: illegal interrupt mode %c\n", TITL, smode[ 0 ] );
         ret = -EINVAL;
         goto err_irq;
   }
   own_proc_dir = create_proc_entry( NAME_DIR, S_IFDIR | S_IRWXUGO, NULL );
   if( NULL == own_proc_dir ) {
      ret = -ENOENT;
      ERR( "%s: can't create directory /proc/%s\n", TITL, NAME_DIR );
      goto err_dir;
   }
   own_proc_dir->uid = own_proc_dir->gid = 0;
   own_proc_node = create_proc_entry( NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO, own_proc_dir );
   if( NULL == own_proc_node ) {
      ret = -ENOENT;
      ERR( "%s: can't create node /proc/%s/%s\n", TITL, NAME_DIR, NAME_NODE );
      goto err_node;
   }
   own_proc_node->uid = own_proc_node->gid = 0;
   own_proc_node->proc_fops = &node_fops;
   LOG( "%s: register node /proc/%s/%s\n", TITL, NAME_DIR, NAME_NODE );
   return 0;
err_node:
   remove_proc_entry( NAME_DIR, NULL );
err_dir:
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
err_irq:
   return ret;
#endif
}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
void cleanup_module( void ) {
   remove_proc_entry( NAME_NODE, own_proc_dir );
   remove_proc_entry( NAME_DIR, NULL );
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
   switch( smode[ 0 ] ) {
      case 't' :
         tasklet_kill( &cnt_tasklet ); 
      case 'w' :
         flush_workqueue( wq );
         destroy_workqueue( wq );
         break;        
      default  :
         break;
   }
   LOG( "%s: module removed: registered %llu events\n", TITL, atomic64_read( &counter ) );
}
#endif

MODULE_LICENSE( "GPL v2" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
