#include <linux/module.h>
#include "mf.h"

static int __init init_driver( void ) { return 0; }

static void __exit cleanup_driver( void ) {}

module_init( init_driver );
module_exit( cleanup_driver );
