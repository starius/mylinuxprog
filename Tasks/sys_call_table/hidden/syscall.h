#define SYSNUM 5
// номера для новых системных вызовов:
static int __NR_own[ SYSNUM ] = { 223, 222, 127, 130, 137 };
// может быть взят любой, полученный при загрузке модуля ni-test.ko
// для ядра 2.6.32 был получен ряд:
// 017, 031, 032, 035, 044, 053, 056, 058, 098, 112, 127, 130, 137,
// 167, 188, 189, 222, 223, 251, 273, 274, 275, 276, 285, 294, 317,
#ifndef SYSCALL
#define SYSCALL 0
#endif

#if SYSCALL>=SYSNUM
#define SYSCALL 0
#endif

#define NR (__NR_own[ SYSCALL ])


