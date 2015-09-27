// VARIANT=7:
// c07a3955 T printk

asmlinkage long new_sys_call( const char __user *buf, size_t count ) {
   long res = 0; 
   asmlinkage int (*own_printk)( const char *fmt, ... );
   struct fix { char str[ 64 ]; } X = { "! STRING FOR PRINTK\n" };
   own_printk  = (void*)0xc07a3955;
   res = own_printk( (const char*)&X );
   asm( "leave            \n"
        "ret              \n"  // эквивалент return res;
        "L2: nop          \n"  // нам нужна метка L2 после return
        ::"a"(res):
      );
   return 0;                   // только для синтаксиса
};
