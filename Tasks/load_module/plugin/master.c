#include <linux/fs.h>
#include <linux/vmalloc.h>
#include "syscall.h"
#include "../common.c"
#include "../find.c"
#include "CR0.c" 

static char this_mod_file[ 40 ];      // имя файла master-модуля

static void **taddr,                  // адрес таблицы sys_call_table 
            *old_sys_addr;            // адрес старого обработчика (sys_ni_syscall)  

asmlinkage long (*sys_init_module)    // системный вызов sys_init_module()
                ( void __user *umod, unsigned long len, const char __user *uargs );
asmlinkage long (*sys_delete_module)  // системный вызов sys_delete_module()
                ( const char __user *name, unsigned int flags );

static long load_slave( const char* fname ) {
   long res = 0; 
   struct file *f;
   long len;
   void *buff;
   size_t n;
   f = filp_open( fname, O_RDONLY, 0 );
   if( IS_ERR( f ) ) {
      printk( "+ ошибка открытия файла %s\n", fname );
      return -ENOENT;
   }
   len = vfs_llseek( f, 0L, 2 ); // 2 - means SEEK_END 
   if( len <= 0 ) {
      printk( "+ ошибка lseek\n" );
      return -EINVAL;
   }
   printk( "+ длина файла модуля = %d байт\n", (int)len );
   if( NULL == ( buff = vmalloc( len ) ) ) {
      filp_close( f, NULL );
      return -ENOMEM;
   };
   printk( "+ адрес буфера чтения = %p\n", buff );
   vfs_llseek( f, 0L, 0 );       // 0 - means SEEK_SET 
   n = kernel_read( f, 0, buff, len );
   printk( "+ считано из файла %s %d байт\n", fname, n );
   if( n != len ) {
      printk( "+ ошибка чтения\n" );
      vfree( buff );
      filp_close( f, NULL );
      return -EIO;
   }
   filp_close( f, NULL );
   { mm_segment_t fs = get_fs();
     set_fs( get_ds() );
     res = sys_init_module( buff, len, "" ); 
     set_fs( fs );
   }
   vfree( buff );
   if( res < 0 ) 
      printk( "+ ошибка загрузки модуля %s : %ld\n", fname, res );
   return res;
}

static long unload_slave( const char* fname ) {
   long res = 0;
   mm_segment_t fs = get_fs();
   set_fs( get_ds() );
   if( strrchr( fname, '.' ) != NULL )
      *strrchr( fname, '.' ) = '\0';
   res = sys_delete_module( fname, 0 ); 
   set_fs( fs );
   if( res < 0 )
      printk( "+ ошибка выгрузки модуля %s\n", fname );
   return res;
}

//long (*loaded_str_translate)( const char *buf );

// новый системный вызов
asmlinkage long sys_str_translate( const char __user *buf, size_t count ) {
   static const char* slave_name[] =              // имена файлов slave-модулей
                      { "dec.ko", "oct.ko", "hex.ko" };
   static char buf_msg[ 80 ], mod_file[ 40 ], *par1;
   int res = copy_from_user( buf_msg, (void*)buf, count ), ind, trs; 
   buf_msg[ count ] = '\0';
   long (*loaded_str_translate)( const char *buf );
   printk( "+ системный запрос %d байт: %s\n", count, buf_msg );
   if( buf_msg[ 0 ] == '0' ) {
      if( buf_msg[ 1 ] == 'x' ) ind = 2; // hex
      else ind = 1;                      // oct
   }
   else if( strchr( "123456789", buf_msg[ 0 ] ) != 0 ) 
      ind = 0;                           //dec
   else return -EINVAL;
   strcpy( mod_file, slave_name[ ind ] );
   par1 = buf_msg + ind;
   if( ( res = load_slave( mod_file ) ) < 0 ) return res;
   if( ( loaded_str_translate = find_sym( "str_translate" ) ) != NULL )
      printk( "+ адрес обработчика = %p\n", loaded_str_translate );
   else {
      printk( "+ str_translate не найден\n" );
      return -EINVAL;
   }
   if( ( trs = loaded_str_translate( par1 ) ) < 0 )
      return trs;
   printk( "+ вычислено значение %d\n", trs );
   res = unload_slave( mod_file );
   if( res < 0 ) return res;
   else return trs;
};

static int __init mod_init( void ) {
   long res = 0;
   void *waddr; 
   set_mod_name( this_mod_file, __FILE__ );
   if( ( taddr = find_sym( "sys_call_table" ) ) != NULL )
      printk( "+ адрес sys_call_table = %p\n", taddr );
   else {
      printk( "+ sys_call_table не найден\n" );
      return -EINVAL;
   }
   old_sys_addr = (void*)taddr[ __NR_str_trans ];
   printk( "+ адрес в позиции %d[__NR_str_trans] = %p\n", __NR_str_trans, old_sys_addr );
   if( ( waddr = find_sym( "sys_ni_syscall" ) ) != NULL ) 
      printk( "+ адрес sys_ni_syscall = %p\n", waddr );
   else {
      printk( "+ sys_ni_syscall не найден\n" );
      return -EINVAL;
   } 
   if( old_sys_addr != waddr ) {
      printk( "+ непонятно! : адреса не совпадают\n" );
      return -EINVAL;
   }
   printk( "+ адрес нового sys_call = %p\n", &sys_str_translate );
   if( ( waddr = find_sym( "sys_init_module" ) ) == NULL ) {
      printk( "+ sys_init_module не найден\n" );
      return -EINVAL;
   }
   printk( "+ адрес sys_init_module = %p\n", waddr );
   sys_init_module = waddr;
   if( ( waddr = find_sym( "sys_delete_module" ) ) == NULL ) {
      printk( "+ sys_delete_module не найден\n" );
      return -EINVAL;
   }
   printk( "+ адрес sys_delete_module = %p\n", waddr );
   sys_delete_module = waddr;
   rw_enable();
   taddr[ __NR_str_trans ] = sys_str_translate;
   rw_disable();
   printk( "+ модуль %s загружен\n", this_mod_file );
   return res;
}

static void __exit mod_exit( void ) {
   printk( "+ адрес syscall при выгрузке = %p\n", (void*)taddr[ __NR_str_trans ] );
   rw_enable();
   taddr[ __NR_str_trans ] = old_sys_addr;
   rw_disable();
   printk( "+ восстановлен адрес syscall = %p\n", old_sys_addr );
   printk( "+ модуль %s выгружен\n", this_mod_file );
   return;
}

