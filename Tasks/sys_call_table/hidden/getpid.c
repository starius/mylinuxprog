// VARIANT=2 :
// c044e690 T sys_getpid

asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 0; 
   long (*own_getpid)( void );
   own_getpid = 0xc044e690UL; 
   res = own_getpid();
   asm( "leave            \n"
        "ret              \n"  // эквивалент return res;
        "L2: nop          \n"  // нам нужна метка L2 после return
        ::"a"(res):
      );
   return 0;                   // только для синтаксиса
};


