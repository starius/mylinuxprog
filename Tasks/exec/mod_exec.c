#include <linux/module.h>

static char *str;
module_param( str, charp, S_IRUGO );

int __init exec_init( void ) {
   int rc;
   char *argv[] = { "wall", "\nthis is wall message ", NULL },
        *envp[] = { NULL },
        msg[ 80 ];
   if( str ) {
      sprintf( msg, "\n%s", str );
      argv[ 1 ] = msg;
   }
   rc = call_usermodehelper( "/usr/bin/wall", argv, envp, 0 );
   if( rc ) {
      printk( KERN_INFO "failed to execute : %s\n", argv[ 0 ] );
      return rc;
   }
   printk( KERN_INFO "execute : %s %s\n", argv[ 0 ], argv[ 1 ] );
   msleep( 100 ); 
   return -1;
}

module_init( exec_init );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_VERSION( "2.1" );
