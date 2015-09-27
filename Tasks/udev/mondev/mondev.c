#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <net/if.h>
#include <linux/netlink.h>

int main() {
   int fd = socket( AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT );  // создаем нужный сокет
   if( fd < 0 ) {
      perror( "Ошибка создания netlink сокета" );
      return 1;
   }
   struct sockaddr_nl	local;                                         // локальный адрес
   memset( &local, 0, sizeof( local ) );                               // очищаем структуру
   local.nl_family = PF_NETLINK;                                       // указываем семейство протокола
   local.nl_groups = 1;
   local.nl_pid = pthread_self() << 16 | getpid(); 
   if( bind( fd, (struct sockaddr*)&local, sizeof( local ) ) < 0 ) {   // связываемся с сокетом
        perror( "Ошибка связывания с netlink сокетом" );
        close( fd );
        return 1;
   }
   char buf[ 8192 ] = "до инициализации";                              // буфер сообщения
   struct iovec iov;                                                   // структура сообщения
   iov.iov_base = &buf;                                                // указываем buf в качестве буфера сообщения для iov
   iov.iov_len = sizeof( buf );                                        // указываем размер буфера
   struct msghdr msg;                                                  // структура сообщения netlink -                  
   msg.msg_name = &local;                                              // задаем имя - структуру локального адреса
   msg.msg_namelen = sizeof( local );                                  // указываем размер
   msg.msg_iov = &iov;                                                 // указываем вектор данных сообщения
   msg.msg_iovlen = 1;                                                 // задаем размеоность вектора
   while( 1 ) {                                                        // читаем и разбираем сообщения из сокета
      ssize_t size = recvmsg( fd, &msg, MSG_DONTWAIT );                // после вызова bind() мы можем принимать сообщения
      if( size < 0 ) {
         if( errno == EINTR || errno == EAGAIN ) 
            usleep( 250000 );
         else   
            perror( "Ошибка приема сообщения netlink" );
         continue;
      }
      printf( "..........................................................\n" );
      char *p = buf;
      while( 1 ) {
         printf( "%s\n", p );
         if( strstr( p, "SEQNUM=" ) == p ) break;
         p += ( strlen( p ) + 1 );
      }; 
   }
   close( fd );                                                        // закрываем дескриптор сокета
   return 0;
}


