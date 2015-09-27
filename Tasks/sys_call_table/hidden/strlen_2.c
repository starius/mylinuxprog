// VARIANT=4:

asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 0; 
   size_t own_strlen( const char* ps ) {
      long res = 0;
      asm( 
        "movl    8(%%ebp), %%eax \n"  // ps -> call parameter 
        "movl    %%eax, (%%esp)  \n"
        "call   *%%ebx           \n" 
        :"=a"(res):"b"((long)(&strlen)):
      );
      return res;
   }
   res = own_strlen( buf );
   asm( "leave            \n"
        "ret              \n"  // эквивалент return res;
        "L2: nop          \n"  // нам нужна метка L2 после return
        ::"a"(res):
      );
   return 0;                   // только для синтаксиса
};
