#include "libdiag.h"

void set_rt( void ) {
   struct sched_param sched_p;            // Information related to scheduling priority
   sched_getparam( getpid(), &sched_p );  // Change the scheduling policy to SCHED_FIFO
   sched_p.sched_priority = 50;           // RT Priority
   sched_setscheduler( getpid(), SCHED_FIFO, &sched_p );
}
