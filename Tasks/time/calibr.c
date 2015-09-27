#include "libdiag.h"

#define NUMB 10
unsigned calibr( int rep ) {
   uint64_t n, m, sum = 0;
   n = m = ( rep >= 0 ? NUMB : rep );
   while( n-- ) {
      uint64_t cf, cs;   
      cf = rdtsc();
      cs = rdtsc();
      sum += cs - cf;
   }
   return (uint32_t)( sum / m );
}
