#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

#define LOG(...) printk( KERN_INFO "=== "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "=== "__VA_ARGS__ )

#define SHARED_IRQ 1
#define NAME_LEN   12
#define IRQ_ID     321

static int irq = SHARED_IRQ;           // контролируемая IRQ 
module_param( irq, int, 0 );

static atomic64_t counter = ATOMIC_LONG_INIT( 0 );
static volatile long unsigned int next; 

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
static irqreturn_t handler( int irq, void *id ) {
   atomic64_inc( &counter );
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
   ERR( "it's work only for kernel LE 3.9\n" );
   return -ECANCELED;
#else
   char dev[ NAME_LEN ];
   int ret;
   struct proc_dir_entry *own_proc_node;
   sprintf( dev, "handler-%02d", irq );
   if( ( ret = request_irq( irq, handler, IRQF_SHARED, dev, (void*)IRQ_ID ) ) != 0 ) {
      ERR( "can't install handler for IRQ=%d\n", irq );
      goto err_irq;
//      return ret;
   }      
   own_proc_dir = create_proc_entry( NAME_DIR, S_IFDIR | S_IRWXUGO, NULL );
   if( NULL == own_proc_dir ) {
      ret = -ENOENT;
      ERR( "can't create directory /proc/%s\n", NAME_DIR );
      goto err_dir;
   }
   own_proc_dir->uid = own_proc_dir->gid = 0;
   own_proc_node = create_proc_entry( NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO, own_proc_dir );
   if( NULL == own_proc_node ) {
      ret = -ENOENT;
      ERR( "can't create node /proc/%s/%s\n", NAME_DIR, NAME_NODE );
      goto err_node;
   }
   own_proc_node->uid = own_proc_node->gid = 0;
   own_proc_node->proc_fops = &node_fops;
   LOG( "register node /proc/%s/%s\n", NAME_DIR, NAME_NODE );
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

void cleanup_module( void ) {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
   remove_proc_entry( NAME_NODE, own_proc_dir );
   remove_proc_entry( NAME_DIR, NULL );
   synchronize_irq( irq );
   free_irq( irq, (void*)IRQ_ID );
   LOG( "module removed: registered %llu events\n", atomic64_read( &counter ) );
#endif
}

MODULE_LICENSE( "GPL v2" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
