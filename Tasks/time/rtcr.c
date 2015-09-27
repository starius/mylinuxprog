#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/rtc.h>

int main( void ) { 
   int fd, retval = 0; 
   struct rtc_time tm;
   memset( &tm, 0, sizeof( struct rtc_time ) );   
   fd = open( "/dev/rtc", O_RDONLY ); 
   if( fd < 0 ) printf( "error: %m\n" );   
   retval = ioctl( fd, RTC_RD_TIME, &tm );  // Чтение времени RTC
   if( retval ) printf( "error: %m\n" );
   printf( "current time: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec );
   close( fd ); 
   return 0; 
}
