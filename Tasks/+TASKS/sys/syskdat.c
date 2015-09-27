#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )

static char* path;                                // имя каталога в /sys/kernel/...
module_param( path, charp, 0 );

// предварительные описания:
static ssize_t show( struct kobject *kobj, struct kobj_attribute *attr, char *buf );
static ssize_t store( struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count );

#define NODES 3
char* title[ NODES ] = { "node1", "node2", "node3" };
static struct kobj_attribute attributes[ NODES ]; // атрибуты-файлы

// Create a group of attributes so that we can create and destroy them all
static struct attribute *attrs[ NODES + 1 ] = {
   &attributes[ 0 ].attr,
   &attributes[ 1 ].attr,
   &attributes[ 2 ].attr,
   NULL,   // need to NULL terminate the list of attributes
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
   .attrs = attrs,
};

static int *target( struct kobj_attribute *attr ) {
   int i;
   static int value[ NODES ] = { 0, 0, 0 };       // данные упакованы вовнутрь
   for( i = 0; i < NODES; i++ )
      if( strcmp( attr->attr.name, attributes[ i ].attr.name ) == 0 )
         return value + i;
   return NULL;
}

static ssize_t show( struct kobject *kobj, struct kobj_attribute *attr, char *buf ) {
   int *pv = target( attr );
   return  NULL == pv ? -EINVAL : sprintf( buf, "%d\n", *pv );;
}

static ssize_t store( struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count) {
   int num, *pv = target( attr );
   if( NULL == pv ) return -EINVAL;
   num =  sscanf( buf, "%d", pv );
   return 0 == num ? -EINVAL : count;
}

static struct kobject *data_kobj;

static int __init proc_init( void ) {
   char name[ 40 ] = "data";                      // каталог по умолчанию
   int i, ret;
   if( NULL != path ) {
      if( strchr( path, '/' ) != NULL ) return -EINVAL;
      else strcpy( name, path );
   }
   // Create a simple kobject with the name
   data_kobj = kobject_create_and_add( name, kernel_kobj );
   if( !data_kobj ) {
      ERR( "cat'n create directory /sys/kernel/%s\n", name );
      return -ENOMEM;
   }
   // Create the files associated with this kobject
   for( i = 0; i < NODES; i++ ) {                 // заполнение имён файлов
      struct kobj_attribute kobj = {
         .attr = { .name = title[ i ], .mode = 0666 },
         .show = show,
         .store = store
      };
      attributes[ i ] = kobj;
   }
   ret = sysfs_create_group( data_kobj, &attr_group );
   if( ret ) {
      ERR( "can't create nodes /sys/kernel/%s/*\n", name );
      kobject_put( data_kobj );
   }
   else LOG( "register nodes /sys/kernel/%s/*\n", name );
   return ret;
}

static void __exit proc_exit( void ) {
   kobject_put( data_kobj );
   LOG( "module removed\n" );
}

module_init( proc_init );
module_exit( proc_exit );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Greg Kroah-Hartman <greg@kroah.com>" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
