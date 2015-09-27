asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   asm( "movl $0, %%eax\n"  // эквивалент return 0;
        "popl %%ebp    \n"
        "ret           \n"
        "L2: nop       \n"  // нам нужна метка L2 после return
        ::: "%eax"
      );
   return 0;                // только для синтаксиса
};

