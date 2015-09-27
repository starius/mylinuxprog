#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

static char* file = "/proc/kallsyms";
module_param( file, charp, 0 ); 
static int debug = 0; 
module_param( debug, int, 0 ); 

static unsigned long hex2adr( char *b ) {
    unsigned long res = 0;
    char *p = b, *d;
    const char *dig = "0123456789abcdef"; 
    while( ( d = strchr( dig, *p ) ) != NULL ) {
        res = ( res << 4 ) + ( d - dig );
        p++; 
    }
    return res; 
}

int verify_close( unsigned long *a ) {
    extern int sys_close( int fd );
    int cmp;
    unsigned long ca = *( a + __NR_close );
    cmp = ( (void*)ca == (void*)sys_close ) ? 0 : 1;
    return cmp; 
} 

void put_table( char *b ) {
    int i;
    char table[ 120 ] = "sys_call_table : "; 
    unsigned long *a;
    unsigned long s = hex2adr( b );
    a = (void*)s;
    printk( "+ sys_call_table address = %p\n", a );
    if( verify_close( a ) != 0 ) {
        printk( "+ sys_close address not valid!\n" );
        return;
    }
    for( i = 0; i < 10; i++ ) {
        unsigned long sa = *( a + i );
        sprintf( table + strlen( table ), "%p ", (void*)sa );
    }
    printk( "+ %s ...\n", table );
}

#define BUF_LEN 255
static int __init stread_init( void ) {
    struct file *f;
    size_t n = 0;
    char buff[ BUF_LEN + 1 ] = "";

    f = filp_open( file, O_RDONLY, 0 );
    if( IS_ERR( f ) ) {
        printk( "+ failed to open: %s\n", file );
        return -ENOENT;
    }
    printk( "+ openning file: %s\n", file );

    while( 1 ) {
       char *p = buff, *find;
       int k;
       *p = '\0';
       do {
           if( ( k = kernel_read( f, n, p++, 1 ) ) < 0 ) {
               printk( "+ failed to read\n" );
               return -EIO;
           };
           *p = '\0';
           if( 0 == k ) break;
           n += k;
           if( '\n' == *( p - 1 ) ) break;
       } while( 1 );
       if( ( debug != 0 ) && ( strlen( buff ) > 0 ) ) {
           if( '\n' == buff[ strlen( buff ) - 1 ] ) printk( "+ %s", buff );
           else printk( "+ %s|\n", buff );
       }
       if( 0 == k ) break;   // EOF

       if( NULL == ( find = strstr( buff, "sys_call_table" ) ) ) continue;
       put_table( buff );
    }

    printk( "+ close file: %s\n", file );
    filp_close( f, NULL );
    return -EPERM;
}

module_init( stread_init );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" ); 
