#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/pci.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )

static char* path;           // имя каталога в /sys/class/...
module_param( path, charp, 0 );

static long value = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t node_show( struct class *class, struct class_attribute *attr, char *buf ) {
#else
static ssize_t node_show( struct class *class, char *buf ) {
#endif
   sprintf( buf, "%ld\n", value );  
   LOG( "read %ld: %s", (long)strlen( buf ), buf );
   return strlen( buf );
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t node_store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {
#else
static ssize_t node_store( struct class *class, const char *buf, size_t count ) {
#endif
   char buf_msg[ 30 ], *endp;
   long res;
   strncpy( buf_msg, buf, count );
   if( '\n' == buf_msg[ count - 1 ] ) buf_msg[ count - 1 ] = '\0';
   else buf_msg[ count ] = '\0';
   LOG( "write %ld: %s\n", count, buf_msg );                 // запрос обновление
   if( 0 == strlen( buf_msg ) ) return -EINVAL;              // уже был возврат ошибки
   res = simple_strtol( buf_msg, &endp, 10 );
   if( strlen( buf_msg ) != endp - buf_msg ) return -EINVAL; // не преобразуемая строка
   value = res;
   LOG( "write: update %ld\n", value );
   return count;
}

CLASS_ATTR( node, ( S_IWUSR | S_IRUGO ), &node_show, &node_store );

static struct class *data_class;

static int __init proc_init( void ) {
   char name[ 40 ] = "data";                                 // каталог по умолчанию
   int ret;
   if( NULL != path ) {
      if( strchr( path, '/' ) != NULL ) return -EINVAL;
      else strcpy( name, path );
   }
   data_class = class_create( THIS_MODULE, name );
   if( IS_ERR( data_class ) ) {
      ERR( "cat'n create directory /sys/class/%s\n", name );
      return -ENOENT;
   }
   ret = class_create_file( data_class, &class_attr_node );
   if( ret != 0 ) {
      ERR( "can't create node /sys/class/%s/node\n", name );
      class_destroy( data_class );
      return -ENOENT;
   }
   LOG( "register node /sys/class/%s/node\n", name );
   return 0;
}

static void __exit proc_exit( void ) {
   class_remove_file( data_class, &class_attr_node );
   class_destroy( data_class );
   LOG( "module removed\n" );
} 

module_init( proc_init );
module_exit( proc_exit ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
