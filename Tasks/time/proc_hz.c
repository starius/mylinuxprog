#include "libdiag.h"
                    
uint64_t proc_hz( void ) {
   time_t t1, t2;
   int64_t cf, cs;
   time( &t1 );
   while( t1 == time( &t2 ) ) cf  = rdtsc(); // начало след. секунды
   while( t2 == time( &t1 ) ) cs  = rdtsc(); // начало след. секунды
   return cs - cf - calibr( 1000 );          // с учётом времени вызова rdtsc() 
}
