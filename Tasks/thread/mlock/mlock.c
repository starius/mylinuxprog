#include <linux/module.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/jiffies.h> 
#include <linux/semaphore.h>

#include "../prefix.c"

static int num = 2;            // num - число рабочих потоков
module_param( num, int, 0 );
static int rep = 100;          // rep - число повторений (объём работы)
module_param( rep, int, 0 );
static int sync = -1;          // sync - уровень на котором синхронизация
module_param( sync, int, 0 );
static int max_level = 2;      // max_level - уровень глубины вложенности
module_param( max_level, int, 0 );

static DECLARE_MUTEX( sema );
static long locked = 0;

static long loop_func( int lvl ) { 
   long n = 0;
   if( lvl == sync ) { down( &sema ); locked++; }
   if( 0 == lvl ) {
      const int tick = 1;
      msleep( tick );                      // выполняемая работа потока 
      n = 1;
   }
   else {
      int i;
      for( i = 0; i < rep; i++ ) {
         n += loop_func( lvl - 1 );
      }
   }
   if( lvl == sync ) up( &sema );
   return n;
}

struct param {                             // параметр потока
   int    num;
   struct completion finished;
};

#define IDENT "mlock_thread_%d"
static int thread_func( void* data ) { 
   long n = 0;
   struct param *parent = (struct param*)data;
   int num = parent->num - 1;             // порядковый номер потока (локальный!)
   struct task_struct *t1 = NULL;
   struct param parm;
   printk( "! %s is running\n", st( num ) );
   if( num > 0 ) {
      init_completion( &parm.finished );
      parm.num = num;
      t1 = kthread_run( thread_func, (void*)&parm, IDENT, num );
   }
   n = loop_func( max_level );            // рекурсивный вызов вложенных циклов
   if( t1 != NULL ) 
      wait_for_completion( &parm.finished );
   complete( &parent->finished );
   printk( "! %s do %ld units\n", st( num ), n );
   return 0;
}

static int test_mlock( void ) { 
   struct task_struct *t1;
   struct param parm;
   unsigned j1 = jiffies, j2;
   if( sync > max_level ) sync = -1;   // без синхронизации
   printk( "! repeat %d times in %d levels; synch. in level %d\n",
           rep, max_level, sync );
   init_completion( &parm.finished );
   parm.num = num;
   t1 = kthread_run( thread_func, (void*)&parm, IDENT, num );
   wait_for_completion( &parm.finished );
   printk( "! %s is finished\n", st( num ) );
   j2 = jiffies - j1;
   printk( "!! working time was %d.%1d seconds, locked %ld times\n",
           j2 / HZ, ( j2 * 10 ) / HZ % 10, locked );
   return -1; 
} 

module_init( test_mlock ); 
MODULE_LICENSE( "GPL" );
