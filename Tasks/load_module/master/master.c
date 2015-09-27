#include <linux/fs.h>
#include <linux/vmalloc.h>
#include "../common.c"
#include "../find.c"

static char* file = "./slave.ko";
module_param( file, charp, 0 );

static char this_mod_file[ 40 ],               // имя файла master-модуля
            slave_name[ 80 ];                  // имя файла slave-модуля

static int __init mod_init( void ) {
   long res = 0, len;
   struct file *f;
   void *buff;
   size_t n;
   asmlinkage long (*sys_init_module)          // системный вызов sys_init_module()
              ( void __user *umod, unsigned long len, const char __user *uargs );
   set_mod_name( this_mod_file, __FILE__ );
   if( ( sys_init_module = find_sym( "sys_init_module" ) ) == NULL ) {
      printk( "! sys_init_module не найден\n" );
      res = -EINVAL;
      goto end;
   }
   printk( "+ адрес sys_init_module = %p\n", sys_init_module );
   strcpy( slave_name, file );
   f = filp_open( slave_name, O_RDONLY, 0 );
   if( IS_ERR( f ) ) {
      printk( "+ ошибка открытия файла %s\n", slave_name );
      res = -ENOENT;
      goto end;
   }
   len = vfs_llseek( f, 0L, 2 ); // 2 - means SEEK_END 
   if( len <= 0 ) {
      printk( "+ ошибка lseek\n" );
      res = -EINVAL;
      goto close;
   }
   printk( "+ длина файла модуля = %d байт\n", (int)len );
   if( NULL == ( buff = vmalloc( len ) ) ) {
      res = -ENOMEM;
      goto close;
   };
   printk( "+ адрес буфера чтения = %p\n", buff );
   vfs_llseek( f, 0L, 0 );       // 0 - means SEEK_SET 
   n = kernel_read( f, 0, buff, len );
   printk( "+ считано из файла %s %d байт\n", slave_name, n );
   if( n != len ) {
      printk( "+ ошибка чтения\n" );
      res = -EIO;
      goto free;
   }
   { mm_segment_t fs = get_fs();
     set_fs( get_ds() );
     res = sys_init_module( buff, len, "" ); 
     set_fs( fs );
     if( res < 0 ) goto insmod;
   }
   printk( "+ модуль %s загружен: file=%s\n", this_mod_file, file );
insmod:
free:
   vfree( buff );
close:
   filp_close( f, NULL );
end: 
   return res;
}

static void __exit mod_exit( void ) {
   asmlinkage long (*sys_delete_module)        // системный вызов sys_delete_module()
              ( const char __user *name, unsigned int flags );
// flags: O_TRUNC, O_NONBLOCK
   char *slave_mod = strrchr( slave_name, '/' ) != NULL ?
                     strrchr( slave_name, '/' ) + 1 :
                     slave_name;
   *strrchr( slave_mod, '.' ) = '\0';
   printk( "+ выгружается модуль %s\n", slave_mod );
   if( ( sys_delete_module = find_sym( "sys_delete_module" ) ) == NULL ) {
      printk( "! sys_delete_module не найден\n" );
      return;
   }
   printk( "+ адрес sys_delete_module = %p\n", sys_delete_module );
   { long res = 0;
     mm_segment_t fs = get_fs();
     set_fs( get_ds() );
     res = sys_delete_module( slave_mod, 0 ); 
     set_fs( fs );
     if( res < 0 )
        printk( "+ ошибка выгрузки модуля %s\n", slave_mod );
   }
   printk( "+ модуль %s выгружен\n", this_mod_file );
}

