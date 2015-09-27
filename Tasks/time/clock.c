#include "libdiag.h"

int main( int argc, char *argv[] ) {
   printf( "%016llX\n", rdtsc() );
   printf( "%016llX\n", rdtsc() );   
   printf( "%016llX\n", rdtsc() ); 
   printf( "%lld\n", proc_hz() );  
   return EXIT_SUCCESS;
};
