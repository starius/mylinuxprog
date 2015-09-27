#ifndef COMMON_H
#define COMMON_H

#define MAX_LEN 30
#define NPTS 3

static long value = 0,                      // регулируемое значение
       multy = 100;                         // петлевое усиление (в %)
static const char *directory = "asu"; 
static const char *points[ NPTS ] = { "value", "increment", "multiply" };

#ifdef __KERNEL__                          // only kernel space
#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR "! "__VA_ARGS__ )

static long multiplier = 0;
module_param( multiplier, long, 0 );
#endif

#endif
