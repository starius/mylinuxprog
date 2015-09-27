// описания функций обработчиков для разных вариантов:
// asmlinkage long new_sys_call( const char __user *buf, size_t count )
#define VARNUM 7
#ifndef VARIANT
#define VARIANT 0
#endif
#if VARIANT>VARNUM
#undef VARIANT
#define VARIANT 0
#endif

#if VARIANT == 0
   #include "null.c" 
#elif VARIANT == 1
   #include "local.c" 
#elif VARIANT == 2
   #include "getpid.c" 
#elif VARIANT == 3 
   #include "strlen_1.c" 
#elif VARIANT == 4
   #include "strlen_2.c" 
#elif VARIANT == 5
   #include "write.c" 
#elif VARIANT == 6
   #include "strlen_3.c" 
#elif VARIANT == 7
   #include "printk.c" 
#else 
   #include "null.c" 
#endif
