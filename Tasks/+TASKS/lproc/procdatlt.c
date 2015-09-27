#include <linux/module.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )

static char* path;                                   // путевое имя от /proc
module_param( path, charp, 0 );

static long value = 0;

static ssize_t node_read( struct file *file,         // чтение из /proc/<path>:
                          char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ 30 ];
   if( *ppos != 0 ) return 0;                        // EOF
   sprintf( buf_msg, "%ld\n", value );  
   if( copy_to_user( buf, buf_msg, strlen( buf_msg ) ) ) return -EFAULT;
   LOG( "read: return %s", buf_msg );
   *ppos = strlen( buf_msg );
   return strlen( buf_msg );
}

static ssize_t node_write( struct file *file,        // запись в /proc/<path>:
                           const char *buf, size_t count, loff_t *ppos ) {
   char buf_msg[ 30 ], *endp;
   long res;
   int len = count < sizeof( buf_msg ) - 1 ? count : sizeof( buf_msg ) - 1;
   if( copy_from_user( buf_msg, buf, len ) ) return -EFAULT;
   if( '\n' == buf_msg[ len -1 ] ) buf_msg[ len -1 ] = '\0'; 
   else buf_msg[ len ] = '\0';
   res = simple_strtol( buf_msg, &endp, 10 );
   if( strlen( buf_msg ) != endp - buf_msg ) return -EINVAL;
   LOG( "write: update %s\n", buf_msg );
   value = res;
   return len;
}

static const struct file_operations node_fops = {
   .owner = THIS_MODULE,
   .read  = node_read,
   .write  = node_write
};

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
static struct proc_dir_entry *last_proc_node = NULL; // нижний домен имени
#endif

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
static void erase( struct proc_dir_entry *node ) {   // удаление иерархии 
   struct proc_dir_entry *parent;
   do {
      parent = node->parent;
      LOG( "remove entry %s - parent %s\n", node->name, node->parent->name );
      remove_proc_entry( node->name, parent );
      node = parent;
   } while( strcmp( parent->name, "/proc" ) != 0 ); 
}
#endif

static int __init proc_init( void ) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
   ERR( "it's work only for kernel LE 3.9\n" );
   return -ECANCELED;
#else
   char name[ 40 ] = "data";                          // имя по умолчанию
   struct proc_dir_entry *parent_proc_node = NULL;
   if( NULL != path ) {
      char *beg = path, *fin;
      while( 1 ) {
         if( ( fin = strchr( beg, '/' ) ) != NULL ) { // каталог
            strncpy( name, beg, fin - beg );
            name[ fin - beg ] = '\0';     
            last_proc_node = create_proc_entry( name, S_IFDIR | S_IRWXUGO, parent_proc_node );
         }
         else {                                       // терминал
            strcpy( name, beg );
            last_proc_node = create_proc_entry( name, S_IFREG | S_IRUGO | S_IWUGO, parent_proc_node );
         }
         if( NULL == last_proc_node ) {
            ERR( "can't create %s %s\n",
                 ( fin != NULL ? "directory" : "node" ), name );
            erase( parent_proc_node );
            return -ENOENT;
         }
         LOG( "create %s: %s\n",
              ( fin != NULL ? "directory" : "node" ), name );
         last_proc_node->uid = last_proc_node->gid = 0;
         if( NULL == fin ) {
            last_proc_node->proc_fops = &node_fops;
            break; 
         }
         else {
            parent_proc_node = last_proc_node;
            beg = fin + 1;
         }
      } 
   }
   LOG( "module installed\n" );
   return 0;
#endif
}

static void __exit proc_exit( void ) {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
   erase( last_proc_node );
   LOG( "module removed\n" );
#endif
} 

module_init( proc_init );
module_exit( proc_exit ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
