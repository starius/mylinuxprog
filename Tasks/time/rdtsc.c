#include "libdiag.h"

uint64_t rdtsc( void ) {
   union sc {
      struct { uint32_t lo, hi; } r;
      uint64_t f;
   } sc;
   __asm__ __volatile__ ( "rdtsc" : "=a"( sc.r.lo ), "=d"( sc.r.hi ) );
   return sc.f;
}

// вариант для старых GCC:
// __asm__ __volatile__ ( ".byte 0x0f, 0x31" : "=a"( sc.r.lo ), "=d"( sc.r.hi ) );

// вариант но только для 32-бит i686:
// asm volatile ( "rdtsc" : "=A" (x) );

