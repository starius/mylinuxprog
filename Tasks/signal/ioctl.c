#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "ioctl.h"

static void sig_handler( int signo ) {
   printf( "---> signal %d\n", signo );
}

int main( int argc, char *argv[] ) {
   int fd, rc;
   unsigned long pid, sig;
   char *nodename = "/dev/mycdrv";
   pid = getpid();
   sig = SIGDEFAULT;
   if( argc > 1 ) sig = atoi( argv[ 1 ] );
   if( argc > 2 ) pid = atoi( argv[ 2 ] );
   if( argc > 3 ) nodename = argv[ 3 ];
   if( SIG_ERR == signal( sig, sig_handler ) )
      printf( "set signal handler error\n" );
   /* open the device node */
   fd = open( nodename, O_RDWR );
   printf( "I opened the device node, file descriptor = %d\n", fd );
   /* send the IOCTL to set the PID */
   rc = ioctl( fd, MYIOC_SETPID, pid );
   printf("rc from ioctl setting pid is = %d\n", rc );
   /* send the IOCTL to set the signal */
   rc = ioctl( fd, MYIOC_SETSIG, sig );
   printf("rc from ioctl setting signal is = %d\n", rc );
   /* send the IOCTL to send the signal */
   rc = ioctl( fd, MYIOC_SENDSIG, "anything" );
   printf("rc from ioctl sending signal is = %d\n", rc );
   /* ok go home */
   close( fd );
   printf( "FINISHED, TERMINATING NORMALLY\n");
   exit( 0 );
}
