#include <linux/module.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

//#include <errno.h>
#include <linux/unistd.h>
//#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sysdep/tls.h>

void do_write( void ) {
   char *str = "эталонная строка для вывода!\n";
   size_t len = strlen( str ) + 1; 
//   printf( "string for write length = %d\n", len );
//   long n = sys_write( 1, str, len );
//   sys_write( 1, str, len );
   syscall(  __NR_write, 1, str, len );  
//__NR_get_thread_area, &info) == 0) 
//   n = syscall( __NR_write, 1, str, len );  
//   printf( "write return : %d\n", n );
}

/*
void do_mknod( void ) {
   char *nam = "ZZZ";
   int n = syscall( __NR_mknod, nam, S_IFCHR | S_IRUSR | S_IWUSR, MKDEV( 247, 0 ) );
   printf( "mknod return : %d\n", n );
}

void do_getpid( void ) {
   int n = syscall( __NR_getpid );
   printf( "getpid return : %d\n", n );
}
*/

static int __init x80_init( void ) {
   mm_segment_t fs = get_fs();
   set_fs( get_ds() );
   do_write();
//   do_mknod();
//   do_getpid();
   set_fs(fs);
   return -1;
}

module_init( x80_init );

MODULE_LICENSE( "GPL" );


