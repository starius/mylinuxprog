#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> 
#include <linux/slab.h>
#include <../arch/x86/include/asm/cacheflush.h>
#include <linux/init.h>

#include "../find.c" 
#include "../CR0.c" 
#include "syscall.h" 

static long shift; // величина сдвига тела функции системного обработчика 
#include "variant.h" 

static int __init new_sys_init( void ) {
   void *waddr, *move_sys_call, 
       **taddr;                   // адрес таблицы sys_call_table 
   size_t sys_size;
   printk( "!... SYSCALL=%d, VARIANT=%d\n", NR, VARIANT );  
   if( ( taddr = find_sym( "sys_call_table" ) ) != NULL )
      printk( "! адрес sys_call_table = %p\n", taddr );
   else 
      return -EINVAL | printk( "! sys_call_table не найден\n" );
   if( NULL == ( waddr = find_sym( "sys_ni_syscall" ) ) )
      return -EINVAL | printk( "! sys_ni_syscall не найден\n" );
   if( taddr[ NR ] != waddr )
      return -EINVAL | printk( "! системный вызов %d занят\n", NR );
   {  unsigned long end; 
      asm( "movl $L2, %%eax \n"
           :"=a"(end)::  
         ); 
      sys_size = end - (long)new_sys_call;
      printk( "! статическая функция: начало= %p, конец=%lx, длина=%d \n",
              new_sys_call, end, sys_size );

   }
   // выделяем блок памяти под функцию обработчик
   move_sys_call = kmalloc( sys_size, GFP_KERNEL );
   if( !move_sys_call ) return -EINVAL | printk( "! memory allocation error!\n" );
   printk( "! выделен блок %d байт с адреса %p\n", sys_size, move_sys_call );
   // копируем резидентный код нового обработчика в выделенный блок памяти
   memcpy( move_sys_call, new_sys_call, sys_size );
   shift = move_sys_call - (void*)new_sys_call;  
   printk( "! сдвиг кода = %lx байт\n", shift );
   // снять бит NX-защиты с страницы
   //int set_memory_x(unsigned long addr, int numpages);
   set_memory_x( (long unsigned)move_sys_call, sys_size / PAGE_SIZE + 1 );
   printk( "! адрес нового sys_call = %p\n", move_sys_call );
   rw_enable();
   taddr[ NR ] = move_sys_call;
   rw_disable();
   return -EPERM;
}
module_init( new_sys_init );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
