#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>

static char* file = NULL;
module_param( file, charp, 0 );

#define BUF_LEN 255
#define DEFNAME "/etc/yumex.profiles.conf";
static char buff[ BUF_LEN + 1 ] = DEFNAME;

static int __init kread_init( void ) {
    struct file *f;
    size_t n;
    if( file != NULL ) strcpy( buff, file );
    printk( "*** openning file: %s\n", buff );
    f = filp_open( buff, O_RDONLY, 0 );

    if( IS_ERR( f ) ) { 
        printk( "*** file open failed: %s\n", buff );
        return -ENOENT;
    }

    n = kernel_read( f, 0, buff, BUF_LEN );
    if( n ) {
        printk( "*** read first %d bytes:\n", (int)n );
        buff[ n ] = '\0';
        printk( "%s\n", buff );
    } else {
        printk( "*** kernel_read failed\n" );
        return -EIO;
    }

    printk( "*** close file\n" );
//    filp_close( f, current->files );
    filp_close( f, NULL );
    return -EPERM;
}

module_init( kread_init );
MODULE_LICENSE( "GPL" );
