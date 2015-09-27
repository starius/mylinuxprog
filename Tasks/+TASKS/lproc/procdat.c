#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )

#define DEFAULT "data"
static char path[ 80 ] = DEFAULT;                    // путевое имя от /proc
module_param_string( path, path, sizeof( path ), 0 );

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

static void erase( struct proc_dir_entry *node ) {    // удаление иерархии 
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
   struct proc_dir_entry *parent;
   do {
      parent = node->parent;
      LOG( "remove entry %s\n", node->name );
      remove_proc_entry( node->name, parent );
      node = parent;
   } while( strcmp( node->name, "/proc" ) != 0 );
#else
   char *beg = path, *fin, name[ 20 ];                // доменный компонент имени 
   while( 1 ) {
      if( NULL == ( fin = strchr( beg, '/' ) ) ) break;  
      if( fin != beg ) break;
      beg++;
   }
   if( NULL == fin ) 
      strcpy( name, beg );
   else {
      strncpy( name, beg, fin - beg );
      name[ fin - beg ] = '\0';
   }
   remove_proc_subtree( name, NULL );
   LOG( "remove subtree /proc/%s\n", name );
#endif
}

static struct proc_dir_entry *last_proc_node = NULL;  // нижний уровень

static int __init proc_init( void ) {
   char *beg = path, *fin, name[ 20 ];                // доменный компонент имени 
   struct proc_dir_entry *parent_proc_node = NULL;
   if( '/' == path[ strlen( path ) - 1 ] )            // страховка
      path[ strlen( path ) - 1 ] = '\0';
   while( 1 ) {                                       
      if( NULL == ( fin = strchr( beg, '/' ) ) )
         break;                                       // не каталог  
      if( fin == beg ) {
         beg++;
         continue;
      }
      strncpy( name, beg, fin - beg );
      name[ fin - beg ] = '\0';     
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
      last_proc_node = create_proc_entry( 
                          name, S_IFDIR | S_IRWXUGO, parent_proc_node );
      if( last_proc_node != NULL ) 
         last_proc_node->uid = last_proc_node->gid = 0;
#else
      last_proc_node = proc_mkdir( name, parent_proc_node );
#endif
      if( NULL == last_proc_node ) {
         ERR( "can't create directory %s\n", name );
         goto err;
      }
      LOG( "create directory: %s\n", name );
      parent_proc_node = last_proc_node;
      beg = fin + 1;
   }
   strcpy( name, beg );                               // терминальное имя
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 0))
   last_proc_node = create_proc_entry(
                       name, S_IFREG | S_IRUGO | S_IWUGO, parent_proc_node );
   if( last_proc_node != NULL ) {
      last_proc_node->uid = last_proc_node->gid = 0;
      last_proc_node->proc_fops = &node_fops;
   }
#else
   last_proc_node = proc_create( 
                       name, S_IFREG | S_IRUGO | S_IWUGO, parent_proc_node, &node_fops );
#endif
   if( NULL == last_proc_node ) {
      ERR( "can't create node %s\n", name );
      goto err;
   }
   LOG( "create node: %s\n", name );
   return 0;
err:
   erase( parent_proc_node );
   return -ENOENT;
}

static void __exit proc_exit( void ) {
   erase( last_proc_node );
   LOG( "erased /proc/%s\n", path );
} 

module_init( proc_init );
module_exit( proc_exit ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
