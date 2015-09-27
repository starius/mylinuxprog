#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

// пример из http://habrahabr.ru/post/121254/ с минимальными изменениями :
// нотификация сетевых интерфейсов.

// небольшая вспомогательная функция, которая с помощью макрасов netlink разбирает сообщение и 
// помещает блоки данных в массив атрибутов rtattr
void parseRtattr( struct rtattr *tb[], int max, struct rtattr *rta, int len ) {
   memset( tb, 0, sizeof( struct rtattr * ) * ( max + 1 ) );
   while( RTA_OK( rta, len ) ) {                      // пока сообщение не закончилось
      if( rta->rta_type <= max ) {
         tb[ rta->rta_type ] = rta;                   //читаем атрибут
      }
      rta = RTA_NEXT(rta,len);                        // получаем следующий атрибут
   }
}

int main() {
   int fd = socket( PF_NETLINK, SOCK_RAW, NETLINK_ROUTE );   // создаем нужный сокет
   if( fd < 0 ) {
      printf( "Ошибка создания netlink сокета: %s\n", (char*)strerror( errno ) );
      return 1;
   }
   char buf[ 8192 ]; 
   struct iovec iov;                                         // структура сообщения
   iov.iov_base = buf;                                       // указываем buf в качестве буфера сообщения для iov
   iov.iov_len = sizeof( buf );                              // указываем размер буфера
   struct sockaddr_nl	local;                               // локальный адрес
   memset( &local, 0, sizeof( local ) );                     // очищаем структуру
   local.nl_family = AF_NETLINK;		// указываем семейство протокола
   local.nl_groups = 	RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;	// указываем необходимые группы
   local.nl_pid = getpid();	//в качестве идентификатора указываем идентификатор данного приложения

   struct msghdr msg;                                        // структура сообщения netlink - инициализируем все поля
   {  msg.msg_name = &local;                                 // задаем имя - структуру локального адреса
      msg.msg_namelen = sizeof( local );                     // указываем размер
      msg.msg_iov = &iov;                                    // указываем вектор данных сообщения
      msg.msg_iovlen = 1;                                    // задаем длину вектора
   }	

   if( bind( fd, (struct sockaddr*)&local, sizeof( local ) ) < 0 ) {		// связываемся с сокетом
      printf( "Ошибка связывания с netlink сокетом: %s\n", (char*)strerror( errno ) );
      close( fd );
      return 1;
   }	
   
   while( 1 ) {                                              // читаем и разбираем сообщения из сокета
      ssize_t status = recvmsg( fd, &msg, MSG_DONTWAIT );    // после вызова bind() мы можем принимать сообщения для указанных групп
      if( status < 0 ) {                                     // небольшие проверки
         if( errno == EINTR || errno == EAGAIN ) {
            usleep( 250000 );
            continue;
         }
         printf( "Ошибка приема сообщения netlink: %s\n", (char*)strerror( errno ) );
         continue;
      }
      if( msg.msg_namelen != sizeof( local ) ) {              //провряем длину адреса, мало ли :)
         printf( "Некорректная длина адреса отправителя\n" );
         continue;
      }
      // собственно разбор сообщения
      struct nlmsghdr *h;                                     // указатель на заголовок сообщения
      char buf[ 1024 * 1024 ];                                         // буфер сообщения

      for( h = (struct nlmsghdr*)buf; status >= (ssize_t)sizeof( *h ); ) {  // для всех заголовков сообщений
         int len = h->nlmsg_len;         // длина всего блока
         int l = len - sizeof( *h );     // длина текущего сообщения
         char *ifName;                   // имя соединения
         if( ( l < 0 ) || ( len > status ) ) {
            printf( "Некорректная длина сообщения: %i\n", len );
            continue;
         }
         // смотрим тип сообщения
         if( ( h->nlmsg_type == RTM_NEWROUTE ) || ( h->nlmsg_type == RTM_DELROUTE ) ) {   // если это изменения роутов - печатаем сообщение
            printf( "Произошли изменения в таблице маршрутизации\n" );	
         } else {                                  // в остальных случаях начинаем более детально разбираться
            char *ifUpp;                           // состояние устройства
            char *ifRunn;                          // состояние соединения
            struct ifinfomsg *ifi;                 // указатель на структуру, содержащюю данные о сетевом подключении
            struct rtattr *tb[ IFLA_MAX + 1 ];     // массив атрибутов соединения, IFLA_MAX определен в rtnetlink.h
            ifi = (struct ifinfomsg*)NLMSG_DATA( h );                    // получаем информацию о сетевом соединении в кором произошли изменения
            parseRtattr( tb, IFLA_MAX, IFLA_RTA( ifi ), h->nlmsg_len );  // получаем атрибуты сетевого соединения       
            if( tb[ IFLA_IFNAME ] ) {                                    // проверяем валидность атрибута, хранящего имя соединения
            ifName = (char*)RTA_DATA( tb[ IFLA_IFNAME ] );               //получаем имя соединения
         }
         if( ifi->ifi_flags & IFF_UP ) {           // получаем состояние флага UP для соединения
            ifUpp = (char*)"UP";
         } else {
            ifUpp = (char*)"DOWN";
         }
         if( ifi->ifi_flags & IFF_RUNNING ) {      // получаем состояние флага RUNNING для соединения
            ifRunn = (char*)"RUNNING";
         } else {
            ifRunn = (char*)"NOT RUNNING";
         }
         char ifAddress[ 256 ];                    // сетевой адрес интерфейса
         struct ifaddrmsg *ifa;                    // указатель на структуру содержащую данные о сетевом интерфейсе
         struct rtattr *tba[ IFA_MAX+1 ];          // массив атрибутов адреса
         ifa = (struct ifaddrmsg*)NLMSG_DATA( h ); // получаем данные из соединения
         parseRtattr( tba, IFA_MAX, IFA_RTA(ifa ), h->nlmsg_len); // получаем атрибуты сетевого соединения
         if( tba[ IFA_LOCAL ] ) {                  // проверяем валидность указателя локального адреса
             inet_ntop( AF_INET, RTA_DATA( tba[ IFA_LOCAL ] ), ifAddress, sizeof( ifAddress ) ); // получаем IP адрес
         }
         switch( h->nlmsg_type ) {                 //что конкретно произошло
            case RTM_DELADDR:
               printf( "Был удален адрес интерфейса %s\n", ifName );
               break;
            case RTM_DELLINK:
               printf( "Был удален интерфейс %s\n", ifName );
               break;
            case RTM_NEWLINK:
               printf( "Новый интерфейс %s, состояние интерфейса %s %s\n", ifName, ifUpp, ifRunn );
               break;
            case RTM_NEWADDR:
               printf( "Был добавлен адрес для интерфейса %s: %s\n", ifName, ifAddress );
               break;
            }
         }
         status -= NLMSG_ALIGN( len );    // выравниваемся по длине сообщения (если этого не сделать - будет очень плохо, можете проверить :))
         h = (struct nlmsghdr*)( (char*)h + NLMSG_ALIGN( len ) ); //получаем следующее сообщение
      }
      usleep(250000);	// немножко спим, чтобы не очень грузить процессор
   }
   close(fd);	// закрываем дескриптор сокета
   return 0;
}
