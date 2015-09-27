#include "mod_proc.h"
#include "proc_node_read.c" // чтение-запись read_proc_t /proc/mod_dir/mod_node
#include "fops_rw.c"        // чтение-запись fops для /proc/mod_dir/mod_node

static int mode = 0; 
module_param( mode, int, S_IRUGO );

static const struct file_operations node_fops = {
   .owner = THIS_MODULE,
   .read  = node_read,
};

static int __init proc_init( void ) {
   int ret;
   struct proc_dir_entry *own_proc_node;
   own_proc_node = create_proc_entry( NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO, NULL );
   if( NULL == own_proc_node ) {
      ret = -ENOENT;
      ERR( "can't create /proc/%s", NAME_NODE );
      goto err_node;
   }
   own_proc_node->uid = own_proc_node->gid = 0;
   if( mode !=1 )
      own_proc_node->read_proc = proc_node_read;
   if( mode !=2 )
      own_proc_node->proc_fops = &node_fops;
   LOG( "/proc/%s installed", NAME_NODE );
   return 0;
err_node:
  return ret;
}

static void __exit proc_exit( void ) {
   remove_proc_entry( NAME_NODE, NULL );
   LOG( "/proc/%s removed", NAME_NODE );
}

