#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/mman.h>
#include "libdiag.h"

unsigned long ts0, worst = 0, mean = 0;      // для загрузки тиков
unsigned long cali;
unsigned long long sum = 0;                  // для накопления суммы
int cycle = 0;

void do_work( int n ) { 
   unsigned long now = rdtsc();
   now = now - ts0 - cali;
   sum += now;
   if( now > worst ) {
      worst = now;                           // Update the worst case latency
      cycle = n;
   }
   return;
}

int main( int argc, char *argv[] ) {
   int fd, opt, i = 0, rep = 1000, nice = 0, freq = 8192; // freq - RTC частота - hz 
   /* Set the periodic interrupt frequency to 8192Hz
      This should give an interrupt rate of 122uS */
   while ( ( opt = getopt( argc, argv, "f:r:n") ) != -1 ) {
      switch( opt ) {
         case 'f' : if( atoi( optarg ) > 0 ) freq = atoi( optarg ); break;
         case 'r' : if( atoi( optarg ) > 0 ) rep = atoi( optarg ); break;
         case 'n' : nice = 1; break;
         default :
            printf( "usage: %s [-f 2**n] [-r #] [-n]\n", argv[ 0 ] );
            exit( EXIT_FAILURE );
      }
   };
   if( 0 == nice ) set_rt();
   printf( "interrupt period set %.2f us\n", 1000000. / freq );
   mlockall( MCL_CURRENT );                 // Avoid paging and related indeterminism 
   cali = calibr( 10000 );
   fd = open( "/dev/rtc", O_RDONLY );       // Open the RTC
   unsigned long long prochz = proc_hz();
   ioctl( fd, RTC_IRQP_SET, freq );
   ioctl( fd, RTC_PIE_ON, 0 );              // разрешить периодические прерывания
   while ( i++ < rep ) {
      unsigned long data;
      ts0 = rdtsc();
      // блокировать до следующего периодического прерывния
      read( fd, &data, sizeof(unsigned long) ); 
      // выполнять периодическую работу ... измерять латентность
      do_work( i );
   }
   ioctl( fd, RTC_PIE_OFF, 0 );            // запретить периодические прерывания
   printf( "worst latency was %.2f us (on cycle %d)\n", tick2us( prochz, worst ), cycle );
   printf( "mean latency was %.2f us\n", tick2us( prochz, sum / rep ) );
   exit( EXIT_SUCCESS );
}

