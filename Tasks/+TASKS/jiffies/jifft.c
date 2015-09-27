#include <linux/module.h>
#include <linux/init.h>
#include <linux/kallsyms.h> 
#include <linux/uaccess.h>

static asmlinkage long (*sys_write) ( unsigned int fd, const char __user *buf, size_t count ); 

int symb_fn( void* data, const char* sym, struct module* mod, unsigned long addr ) { 
   if( 0 == strcmp( (char*)data, sym ) ) { 
      sys_write = (void*)addr;
      return 1; 
   }
   else return 0; 
}; 

int init_module( void ) { 
   char msg[ 120 ];
   mm_segment_t fs = get_fs();
   if( 0 == kallsyms_on_each_symbol( symb_fn, (void*)"sys_write" ) ) { 
      printk( "error: symbol not found" ); 
      return -1; 
   } 
   sprintf( msg, "module: jiffies on start = %lX\n", jiffies );
   set_fs( get_ds() );
   sys_write( 1, msg, strlen( msg ) ); 
   set_fs(fs);
   return -1;
} 

MODULE_LICENSE( "GPL" ); // не убирать - не сработает!
