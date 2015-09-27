asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 1000;            
   int inc( int in ) {           // вложенное описание функции 
      return ++in;
   }
   res = res + inc( count );
   res = inc( count );
   asm( "movl %%ebx, %%eax\n\t"  // эквивалент return res;
        "leave            \n\t"
        "ret              \n\t"
        "L2: nop          \n\t"  // нам нужна метка L2 после return
        ::"b"(res):"%eax"
      );
   return 0;                     // только для синтаксиса
};

