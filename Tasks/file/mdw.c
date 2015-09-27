#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static char* log = NULL;
module_param( log, charp, 0 );

#define BUF_LEN 255
#define DEFLOG "./module.log"
#define TEXT "...............\n"

static struct file *f;

static int __init init( void ) {
   ssize_t n = 0;
   loff_t offset = 0;
   mm_segment_t fs;
   char buff[ BUF_LEN + 1 ] = DEFLOG;
   if( log != NULL ) strcpy( buff, log );
   f = filp_open( buff,
// O_CREAT | O_WRONLY | O_TRUNC, 
                  O_CREAT | O_RDWR | O_TRUNC, 
                  S_IRUSR | S_IWUSR );
   if( IS_ERR( f ) ) { 
      printk( "! file open failed: %s\n", buff );
      return -ENOENT;
   }
   printk( "! file open %s\n", buff );
   fs = get_fs();
   set_fs( get_ds() );
   strcpy( buff, TEXT );
   if( ( n = vfs_write( f, buff, strlen( buff ), &offset ) ) != strlen( buff ) ) {
        printk( "! failed to write: %d\n", (int)n );
        return -EIO;
   }
   printk( "! write %d bytes\n", (int)n );
   printk( "! %s", buff );
   set_fs( fs );
   filp_close( f, NULL );
   printk( "! file close\n" );
   return -1;
}
module_init( init );
MODULE_LICENSE( "GPL" );
