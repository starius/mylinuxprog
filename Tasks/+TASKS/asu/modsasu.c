#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include "common.h"

// предварительные описания операций:
static ssize_t show( struct kobject *kobj, struct kobj_attribute *attr, char *buf );
static ssize_t store( struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count );

static struct kobj_attribute attributes[ NPTS ] = {
   { .attr = { .name = "value", .mode = 0666 },
     .show = show, .store = store },
   { .attr = { .name = "increment", .mode = 0222 },
     .store = store },
   { .attr = { .name = "multiply", .mode = 0666 },
     .show = show, .store = store }
};

// Create a group of attributes so that we can create and destroy them all
static struct attribute *attrs[ NPTS + 1 ] = {
   &attributes[ 0 ].attr,
   &attributes[ 1 ].attr,
   &attributes[ 2 ].attr,
   NULL,   // need to NULL terminate the list of attributes
};

/* An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.  */
static struct attribute_group attr_group = {
   .attrs = attrs,
};

static long *target( struct kobj_attribute *attr ) {
   static long increment;     // корректирующее воздействие спрятали вовнутрь
   static long* const pv[ NPTS ] = { &value, &increment, &multy };
   int i;
   for( i = 0; i < NPTS; i++ )
      if( strcmp( attr->attr.name, attributes[ i ].attr.name ) == 0 )
         return pv[ i ];
   return NULL;
} 

static ssize_t show( struct kobject *kobj, struct kobj_attribute *attr, char *buf ) {
   ssize_t ret;
   long *pv = target( attr ); 
   if( 0 == strcmp( attr->attr.name, points[ 1 ] ) ) return -EINVAL;
   if( NULL == pv ) return -EINVAL;
   ret = sprintf( buf, "%ld\n", *pv );
   LOG( "read %s: %s", attr->attr.name, buf );
   return ret;
}

static ssize_t store( struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count) {
   long num, *pv = target( attr );
   if( NULL == pv ) return -EINVAL;
   num = sscanf( buf, "%ld", pv );
   if( 0 == num ) return -EINVAL;
   LOG( "write %s: %ld\n", attr->attr.name, *pv );
   if( 0 == strcmp( attr->attr.name, points[ 1 ] ) ) // increment
      value += ( *pv * multy ) / 100;
   return count;
}

static struct kobject *asu_kobj;

static int __init proc_init( void ) {
   int ret;
   if( multiplier != 0 ) multy = multiplier;
   // Create a simple kobject with the name
   asu_kobj = kobject_create_and_add( directory, kernel_kobj );
   if( !asu_kobj ) {
      ERR( "cat'n create directory /sys/kernel/%s\n", directory );
      return -ENOMEM;
   }
   // Create the files associated with this kobject
   ret = sysfs_create_group( asu_kobj, &attr_group );
   if( ret ) {
      ERR( "can't create nodes /sys/kernel/%s/*\n", directory );
      kobject_put( asu_kobj );
   }
   else LOG( "register nodes /sys/kernel/%s/*\n", directory );
   return ret;
}

static void __exit proc_exit( void ) {
   kobject_put( asu_kobj );
   LOG( "module removed\n" );
}

module_init( proc_init );
module_exit( proc_exit ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Greg Kroah-Hartman <greg@kroah.com>" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
