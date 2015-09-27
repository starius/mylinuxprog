#include "mod_proc.h"
#include "proc_node_read.c"

static int __init proc_init( void ) {
   if( create_proc_read_entry( NAME_NODE, 0, NULL, proc_node_read, NULL ) == 0 ) {
      ERR( "can't create /proc/%s", NAME_NODE );
      return -ENOENT;
   }
   LOG( "/proc/%s installed", NAME_NODE );
   return 0;
}

static void __exit proc_exit( void ) {
   remove_proc_entry( NAME_NODE, NULL );
   LOG( "/proc/%s removed", NAME_NODE );
}
