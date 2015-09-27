#include "slave.c"

static const char digh[] = "0123456789ABCDEF",
                  digl[] = "0123456789abcdef";

long str_translate( const char *buf ) {
   long res = 0;
   const char *p = buf;
   printk( "+ %s : запрос : %s\n", this_mod_file, buf );
   while( *p != '\0' ) {
      char *s;
      int val;
      s = strchr( digh, *p );
      if( s != NULL ) 
         val = s - digh; 
      else {
         s = strchr( digl, *p );
         if( s == NULL ) return -EINVAL;
         val = s - digl; 
      }
      res = res * 16 + val;
      p++;
   }
   return res;
};

static int __init mod_init( void ) {
   set_mod_name( this_mod_file, __FILE__ );
   printk( "+ модуль %s загружен\n", this_mod_file );
   return 0;
}

static void __exit mod_exit( void ) {
   printk( "+ модуль %s выгружен\n", this_mod_file );
}
