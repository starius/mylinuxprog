static int parameter = 999;

static ssize_t proc_node_write( struct file *file, const char __user *buffer, 
                                unsigned long count, void *data ) {
   unsigned long len = min( count, size );
   LOG( "write: %ld (buffer=%p)", count, buffer );
   memset( buf_msg, NULL_CHAR, length );        // обнуление прежнего содержимого
   length = len;
   if( copy_from_user( buf_msg, buffer, len ) )
      return -EFAULT;
   sscanf( buf_msg, "%d", &parameter );
   LOG( "parameter set to %d", parameter );
   return count;
}
