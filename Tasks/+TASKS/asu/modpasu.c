#include <linux/module.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>
#include "common.h"

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
//-------------------------------------------
static ssize_t value_read( struct file *file,         // чтение из /proc/<path>:
                           char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ MAX_LEN ];
   if( *ppos != 0 ) return 0;                         // EOF
   sprintf( buf_msg, "%ld\n", value );  
   if( copy_to_user( buf, buf_msg, strlen( buf_msg ) ) ) return -EFAULT;
   LOG( "read: return %s", buf_msg );
   *ppos = strlen( buf_msg );
   return strlen( buf_msg );
}

static ssize_t value_write( struct file *file,        // запись в /proc/<path>:
                           const char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ MAX_LEN ], *endp;
   long res;
   int len = count < MAX_LEN - 1 ? count : MAX_LEN - 1;
   if( copy_from_user( buf_msg, buf, len ) ) return -EFAULT;
   if( '\n' == buf_msg[ len -1 ] ) buf_msg[ len -1 ] = '\0'; 
   else buf_msg[ len ] = '\0';
   res = simple_strtol( buf_msg, &endp, 10 );
   if( strlen( buf_msg ) != endp - buf_msg ) return -EINVAL;
   LOG( "write: update %s\n", buf_msg );
   value = res;
   return len;
}
//-------------------------------------------
static ssize_t increment_write( struct file *file,    // запись корректуры
                                const char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ MAX_LEN ], *endp;
   long res;
   int len = count < MAX_LEN - 1 ? count : MAX_LEN - 1;
   if( copy_from_user( buf_msg, buf, len ) ) return -EFAULT;
   if( '\n' == buf_msg[ len -1 ] ) buf_msg[ len -1 ] = '\0'; 
   else buf_msg[ len ] = '\0';
   res = simple_strtol( buf_msg, &endp, 10 );
   if( strlen( buf_msg ) != endp - buf_msg ) return -EINVAL;
   LOG( "write: increment %s\n", buf_msg );
   value += ( ( res * multy ) / 100 );
   return len;
}
//-------------------------------------------
static ssize_t multiply_read( struct file *file,     // чтение петлевого множителя
                              char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ MAX_LEN ];
   if( *ppos != 0 ) return 0;                         // EOF
   sprintf( buf_msg, "%ld\n", multy );  
   if( copy_to_user( buf, buf_msg, strlen( buf_msg ) ) ) return -EFAULT;
   LOG( "read: return %s", buf_msg );
   *ppos = strlen( buf_msg );
   return strlen( buf_msg );
}

static ssize_t multiply_write( struct file *file,     // запись петлевого множителя
                               const char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ MAX_LEN ], *endp;
   long res;
   int len = count < MAX_LEN - 1 ? count : MAX_LEN - 1;
   if( copy_from_user( buf_msg, buf, len ) ) return -EFAULT;
   if( '\n' == buf_msg[ len -1 ] ) buf_msg[ len -1 ] = '\0'; 
   else buf_msg[ len ] = '\0';
   res = simple_strtol( buf_msg, &endp, 10 );
   if( strlen( buf_msg ) != endp - buf_msg ) return -EINVAL;
   LOG( "write: increment %s\n", buf_msg );
   multy = res;
   return len;
}
//-------------------------------------------
static const struct file_operations node_fops[] = {
   { .owner  = THIS_MODULE,    // /asu/value
     .read   = value_read,
     .write  = value_write
   },
   { .owner  = THIS_MODULE,    // /asu/increment 
     .write  = increment_write
   },
   { .owner  = THIS_MODULE,    // /asu/multiply
     .write  = multiply_write,
     .read   = multiply_read
   },
};

static struct proc_dir_entry *dir_proc_entry,
                             *proc_points_entry[ 3 ];

static void clean( void ) {   // удаление всей иерархии 
   int i;
   for( i = 0; i < NPTS; i++ )
      if( proc_points_entry[ i ] != NULL )
         remove_proc_entry( points[ i ], dir_proc_entry );
   if( dir_proc_entry != NULL )
      remove_proc_entry( directory, NULL );
}
#endif

static int __init proc_init( void ) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
   ERR( "it's work only for kernel LE 3.9\n" );
   return -ECANCELED;
#else
   int ret;
   if( multiplier != 0 ) multy = multiplier;
   dir_proc_entry = create_proc_entry( directory, S_IFDIR | S_IRWXUGO, NULL );
   if( NULL == dir_proc_entry ) {
      ret = -ENOENT;
      ERR( "can't create directory /proc/%s", directory );
      goto err_node;
   }
   dir_proc_entry->uid = dir_proc_entry->gid = 0;
   for( int i = 0; i < NPTS; i++ ) {
      proc_points_entry[ i ] = 
         create_proc_entry( points[ i ], S_IFREG | S_IRUGO | S_IWUGO, dir_proc_entry );
      if( NULL == proc_points_entry[ i ] ) {
         ret = -ENOENT;
         ERR( "can't create node /proc/%s/%s\n", directory, points[ i ] );
         goto err_node;
      }
      proc_points_entry[ i ]->uid = proc_points_entry[ i ]->gid = 0;
      proc_points_entry[ i ]->proc_fops = &node_fops[ i ];
   }
   LOG( "register nodes /proc/%s/*\n", directory );
   return 0;
err_node:
   clean();
   return ret;
#endif
}

static void __exit proc_exit( void ) {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
   clean();
   LOG( "module removed\n" );
#endif
} 

module_init( proc_init );
module_exit( proc_exit ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
