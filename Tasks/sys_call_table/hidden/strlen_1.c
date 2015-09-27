// VARIANT=3:
// c05c64af T strlen

asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 0; 
   size_t (*own_strlen)( const char* );
   own_strlen = (void*)0xc05c64af;
/* res = own_strlen( "1234567" );  // error!
   char str[] = "123456789ABCD";
   res = own_strlen( str );        // error!
*/
   res = own_strlen( buf );
   asm( "leave            \n"
        "ret              \n"  // эквивалент return res;
        "L2: nop          \n"  // нам нужна метка L2 после return
        ::"a"(res):
      );
   return 0;                   // только для синтаксиса
};
