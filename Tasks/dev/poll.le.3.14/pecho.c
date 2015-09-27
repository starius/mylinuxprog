#include "poll.h"

int main( int argc, char *argv[] ) {
   struct parm p = parms( argc, argv, 1 );
   const char *sout = argv[ optind ];
   if( p.vis > 0 ) 
      fprintf( stdout, "nonblocked: %s, multiplexed: %s, string for output: %s\n",
         ( 0 == p.blk ? "yes" : "no" ),
         ( 0 == p.mlt ? "yes" : "no" ),
         argv[ optind ] );
   int dfd = opendev();                       // дескриптор устройства
   if( 0 == p.blk ) nonblock( dfd );
   struct pollfd client[ 1 ] = {
      { .fd = dfd,
        .events = POLLOUT | POLLWRNORM,
      }
   };
   struct timeval t1, t2;
   gettimeofday( &t1, NULL );
   int res;
   if( 0 == p.mlt ) res = poll( client, 1, -1 );
   res = write( dfd, sout, strlen( sout ) ); // запись
   gettimeofday( &t2, NULL );
   fprintf( stdout, "interval %s write %d bytes: ", interval( t1, t2 ), res );
   if( res < 0 ) ERR( "write error: %m\n" );
   else if( 0 == res ) {
      if( errno == EAGAIN )
         fprintf( stdout, "device NOT READY!\n" );
   }
   else fprintf( stdout, "%s\n", sout );
   close( dfd );
   return EXIT_SUCCESS;
};
