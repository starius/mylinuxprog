// VARIANT=5:
//c04e12fc T sys_write

asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 0; 
   asmlinkage size_t (*own_write)( int fd, const char* s, size_t l );
   own_write = (void*)0xc04e12fc;
   res = own_write( 1, buf, count );
// res = own_write( 1, "1234567\n", 8 );  // error -14 : Bad address
   asm( "leave            \n"
        "ret              \n"  // эквивалент return res;
        "L2: nop          \n"  // нам нужна метка L2 после return
        ::"a"(res):
      );
   return 0;                   // только для синтаксиса
};
