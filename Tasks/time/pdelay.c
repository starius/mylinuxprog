#include "libdiag.h"

int main( int argc, char *argv[] ) {
   long dl_nsec[] = { 10000, 100000, 200000, 300000, 500000, 1000000, 1500000, 2000000, 5000000 };
/* millisec.=ticks:   0.01,  0.1,    0.2,    0.3,    0.5,    1.0,     1.5,     2.0,     5.0    */
   int c, i, j, bSync = 0, bActive = 0, cycles = 1000, rep = sizeof( dl_nsec ) / sizeof( dl_nsec[ 0 ] );
   while( ( c = getopt( argc, argv, "astn:r:" ) ) != EOF )
      switch( c ) {
         case 'a': bActive = 1; break;
         case 's': bSync = 1; break;
         case 't': set_rt(); break;
         case 'n': cycles = atoi( optarg ); break;
         case 'r': if( atoi( optarg ) > 0 && atoi( optarg ) < rep ) rep = atoi( optarg ); break;
         default:
            printf( "usage: %s [-a] [-s] [-n cycles] [-r repeats]\n", argv[ 0 ] );
            return EXIT_SUCCESS;
   }
   char *title[] = { "passive", "active" };   
   printf( "%d cycles %s delay [millisec. == tick !] :\n", cycles,
              ( bActive == 0 ? title[ 0 ] : title[ 1 ] ) ); 
   unsigned long long prs = proc_hz();
   printf( "processor speed: %lld hz\n", prs );
   long cali = calibr( 1000 );
   for( j = 0; j < rep; j++ ) {
      const struct timespec sreq = { 0, dl_nsec[ j ] };   //секунды:наносекунды
      unsigned long long rb, ra, ri = 0;
      if( bSync != 0 ) nanosleep( &sreq, NULL );            
      if( bActive == 0 ) {
         for( i = 0; i < cycles; i++ ) {
            rb = rdtsc();         
            nanosleep( &sreq, NULL );
            ra = rdtsc();
            ri += ra - rb - cali;
         }
      }
      else {
         long long wpr = (long long)( ( (double)dl_nsec[ j ] ) / 1e9 * prs );
         for( i = 0; i < cycles; i++ ) {
            rb = rdtsc();
            while( ( ra = rdtsc() ) - rb < wpr ); 
            ri +=  ra - rb - cali;
         }
      }         
      double del = ( (double)ri ) / ( (double)prs );
      printf( "set %5.3f => was %5.3f\n", ( ( (double)dl_nsec[ j ] ) / 1e9 ) * 1e3, del * 1e3 / cycles );   
   }
   return EXIT_SUCCESS;
};
