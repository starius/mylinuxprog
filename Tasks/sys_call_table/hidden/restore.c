#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> 
#include <linux/slab.h>
#include <linux/init.h>

#include "../find.c" 
#include "../CR0.c" 
#include "syscall.h" 

static int __init new_sys_init( void ) {
   void **taddr;                   // адрес таблицы sys_call_table 
   void *waddr, *old_sys_addr;
   if( ( taddr = find_sym( "sys_call_table" ) ) != NULL )
      printk( "! адрес sys_call_table = %p\n", taddr );
   else return -EINVAL | printk( "! sys_call_table не найден\n" );
   if( ( waddr = find_sym( "sys_ni_syscall" ) ) != NULL )
      printk( "! адрес sys_ni_syscall = %p\n", waddr );
   else return -EINVAL | printk( "! sys_ni_syscall не найден\n" );
   old_sys_addr = (void*)taddr[ NR ];
   printk( "! адрес в позиции %d = %p\n", NR, old_sys_addr );

   if( old_sys_addr != waddr ) {
      kfree( old_sys_addr );
      rw_enable();
      taddr[ NR ] = waddr;  // восстановить sys_ni_syscall
      rw_disable();
      printk( "! итоговый адрес обработчика %p\n", taddr[ NR ] );
   }
   else
      printk( "! итоговый адрес обработчика не изменяется\n" );
   return -EPERM;
}
module_init( new_sys_init );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

