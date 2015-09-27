#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> 

#include "../find.c" 
#include "../CR0.c" 
#include "syscall.h" 

// системный вызов с двумя параметрами:
asmlinkage long (*old_sys_addr) ( const char __user *buf, size_t count );

asmlinkage long new_sys_call ( const char __user *buf, size_t count ) {
   static char buf_msg[ 80 ];
   int res = copy_from_user( buf_msg, (void*)buf, count ); 
   buf_msg[ count ] = '\0';
   printk( "! передано %d байт: %s\n", count, buf_msg );
   return res;
};
EXPORT_SYMBOL( new_sys_call );

static void **taddr; // адрес таблицы sys_call_table 

static int __init new_sys_init( void ) {
   void *waddr; 
   if( ( taddr = find_sym( "sys_call_table" ) ) != NULL )
      printk( "! адрес sys_call_table = %p\n", taddr );
   else {
      printk( "! sys_call_table не найден\n" );
      return -EINVAL;
   }
   old_sys_addr = (void*)taddr[ __NR_own ];
   printk( "! адрес в позиции %d[__NR_own] = %p\n", __NR_own, old_sys_addr );
   if( ( waddr = find_sym( "sys_ni_syscall" ) ) != NULL ) 
      printk( "! адрес sys_ni_syscall = %p\n", waddr );
   else {
      printk( "! sys_ni_syscall не найден\n" );
      return -EINVAL;
   } 

   if( old_sys_addr != waddr ) {
      printk( "! непонятно! : адреса не совпадают\n" );
      return -EINVAL;
   }
   printk( "! адрес нового sys_call = %p\n", &new_sys_call );
   rw_enable();
   taddr[ __NR_own ] = new_sys_call;
   rw_disable();
   return 0;
}

static void __exit new_sys_exit( void ) {
   printk( "! адрес sys_call при выгрузке = %p\n", (void*)taddr[ __NR_own ] );
   rw_enable();
   taddr[ __NR_own ] = old_sys_addr;
   rw_disable();
   return;
}

module_init( new_sys_init );
module_exit( new_sys_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

