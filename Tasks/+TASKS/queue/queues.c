#include "common.c"

#define SIZE 1024
static char buffer[ SIZE + 1 ] = "\n";                       // статический буфер

static ssize_t queue_read( struct file *file, char *buf,     // чтение из /dev/queue 
                           size_t count, loff_t *ppos ) {
   int len = strlen( buffer );
   if( count < len ) return -EINVAL;
   if( *ppos != 0 ) {
      LOG( "read %d -> 0\n", (int)count );                    // EOF
      return 0;
   }
   if( copy_to_user( buf, buffer, len ) ) return -EINVAL;
   *ppos = len;
   LOG( "read %d -> %d\n", (int)count, len );
   return len;
}

static ssize_t queue_write( struct file *f, const char *buf, // запись в /dev/queue
                            size_t count, loff_t *ppos ) {
   int len = count < SIZE ? count : SIZE;
   if( copy_from_user( buffer, buf, len ) ) return -EINVAL;
   buffer[ len ] = '\0';
   LOG( "write %d -> %d\n", (int)count, len );
   return len;
}

static int __init dev_init( void ) {
   int ret;
   if( minor != 0 ) queue_dev.minor = minor;
   ret = misc_register( &queue_dev );
   if( ret ) {
      printk( KERN_ERR "=== unable to register misc device\n" );
      goto end;
   }
   printk( KERN_INFO "=== register device 10:%d\n", queue_dev.minor );
end:
   return ret;
}

static void __exit dev_exit( void ) {
   printk( KERN_INFO "=== deregister device 10:%d\n", queue_dev.minor );
   misc_deregister( &queue_dev );
}
