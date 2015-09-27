#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ioctl.h"

static void sig_handler( int signo ) {
   printf( "---> signal %d\n", signo );
}

int main( int argc, char *argv[] ) {
   unsigned long sig = SIGDEFAULT;
   printf( "my own PID is %d\n", getpid() );
   sig = SIGDEFAULT;
   if( argc > 1 ) sig = atoi( argv[ 1 ] );
   if( SIG_ERR == signal( sig, sig_handler ) )
      printf( "set signal handler error\n" );
   while( 1 ) pause();
   exit( 0 );
}
