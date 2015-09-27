#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> 

#include "../find.c" 
#include "../CR0.c" 

asmlinkage long (*old_sys_write) (
       unsigned int fd, const char __user *buf, size_t count );

asmlinkage long new_sys_write (
       unsigned int fd, const char __user *buf, size_t count ) {
   int n;
   if( 1 == fd ) {
      static const char prefix[] = ":-) ";
      mm_segment_t fs = get_fs();
      set_fs( get_ds() );
      n = old_sys_write( 1, prefix, strlen( prefix ) );
      set_fs(fs);
   } 
   n = old_sys_write( fd, buf, count );
   return n;
};
EXPORT_SYMBOL( new_sys_write );

static void **taddr; // адрес таблицы sys_call_table 

static int __init wrchg_init( void ) {
   void *waddr; 
   if( ( taddr = find_sym( "sys_call_table" ) ) != NULL )
      printk( "! адрес sys_call_table = %p\n", taddr );
   else {
      printk( "! sys_call_table не найден\n" );
      return -EINVAL;
   }
   old_sys_write = (void*)taddr[ __NR_write ];
   printk( "! адрес в позиции %d[__NR_write] = %p\n", __NR_write, old_sys_write );
   if( ( waddr = find_sym( "sys_write" ) ) != NULL ) 
      printk( "! адрес sys_write = %p\n", waddr );
   else {
      printk( "! sys_write не найден\n" );
      return -EINVAL;
   }
   if( old_sys_write != waddr ) {
      printk( "! непонятно! : адреса не совпадают\n" );
      return -EINVAL;
   }
   printk( "! адрес нового sys_write = %p\n", &new_sys_write );
   show_cr0();
   rw_enable();
   taddr[ __NR_write ] = new_sys_write;
   show_cr0();
   rw_disable();
   show_cr0();
   return 0;
}

static void __exit wrchg_exit( void ) {
   printk( "! адрес sys_write при выгрузке = %p\n", (void*)taddr[ __NR_write ] );
   rw_enable();
   taddr[ __NR_write ] = old_sys_write;
   rw_disable();
   return;
}

module_init( wrchg_init );
module_exit( wrchg_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );

