/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 */
#include <linux/module.h>
#include "ioctl.h"
#include "miscdev.h"

static int sig_pid = 0;
static struct task_struct *sig_tsk = NULL;
static int sig_tosend = SIGDEFAULT;

static inline long mycdrv_unlocked_ioctl(
    struct file *fp, unsigned int cmd, unsigned long arg ) {
   int retval;
   switch( cmd ) {
      case MYIOC_SETPID:
         sig_pid = (int)arg;
         printk( KERN_INFO "Setting pid to send signals to, sigpid = %d\n", sig_pid );
         /* sig_tsk = find_task_by_vpid (sig_pid); */
         sig_tsk = pid_task( find_vpid( sig_pid ), PIDTYPE_PID );
         break;
      case MYIOC_SETSIG:
         sig_tosend = (int)arg;
         printk( KERN_INFO "Setting signal to send as: %d \n", sig_tosend );
         break;
      case MYIOC_SENDSIG:
         if( !sig_tsk ) {
            printk( KERN_INFO "You haven't set the pid; using current\n" );
            sig_tsk = current;
            sig_pid = (int)current->pid;
         }
         printk( KERN_INFO "Sending signal %d to process ID %d\n", sig_tosend, sig_pid );
         retval = send_sig( sig_tosend, sig_tsk, 0 );
         printk( KERN_INFO "retval = %d\n", retval );
         break;
      default:
         printk( KERN_INFO " got invalid case, CMD=%d\n", cmd );
         return -EINVAL;
   }
   return 0;
}

static const struct file_operations mycdrv_fops = {
   .owner = THIS_MODULE,
   .unlocked_ioctl = mycdrv_unlocked_ioctl,
   .open = mycdrv_generic_open,
   .release = mycdrv_generic_release
};

module_init( my_generic_init );
module_exit( my_generic_exit );

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LDD:1.0 s_13/lab3_ioctl_signal.c");
MODULE_LICENSE("GPL v2");
