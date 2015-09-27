#include "poll.h"

int main( int argc, char *argv[] ) {
   struct parm p = parms( argc, argv, -1 );
   int blk = LEN_MSG; 
   if( optind < argc && atoi( argv[ optind ] ) > 0 )
      blk = atoi( argv[ optind ] );
   if( p.vis > 0 ) 
      fprintf( stdout, "nonblocked: %s, multiplexed: %s, read block size: %s bytes\n",
         ( 0 == p.blk ? "yes" : "no" ),
         ( 0 == p.mlt ? "yes" : "no" ),
         argv[ optind ] );
   int dfd = opendev();                 // дескриптор устройства
   if( 0 == p.blk ) nonblock( dfd );
   struct pollfd client[ 1 ] = {
      { .fd = dfd,
        .events = POLLIN | POLLRDNORM,
      }
   };
   while( 1 ) {
      char buf[ LEN_MSG + 2 ];          // буфер данных
      struct timeval t1, t2;
      int res;
      gettimeofday( &t1, NULL );
      if( 0 == p.mlt ) res = poll( client, 1, -1 );
      res = read( dfd, buf, blk );      // чтение
      gettimeofday( &t2, NULL );
      fprintf( stdout, "interval %s read %d bytes: ", interval( t1, t2 ), res );
      fflush( stdout );
      if( res < 0 ) {
         if( errno == EAGAIN ) {
            fprintf( stdout, "device NOT READY\n" );
            if( p.mlt != 0 ) sleep( 3 );
         } 
         else
            ERR( "read error: %m\n" );
      }
      else if( 0 == res ) {
         fprintf( stdout, "read EOF\n" );
         break;
      }
      else {
         buf[ res ] = '\0';
         fprintf( stdout, "%s\n", buf );
      }
   }
   close( dfd );
   return EXIT_SUCCESS;
};
