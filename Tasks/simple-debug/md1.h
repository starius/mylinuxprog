#include <linux/module.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

extern char* test_01( void );
extern char* test_02( void ); 

static int __init init( void );
module_init( init );


