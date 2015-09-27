#include <linux/module.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/vmalloc.h>

#define LOG(...) printk( KERN_INFO "! "__VA_ARGS__ )

static int thr = 2;          // число рабочих потоков
module_param( thr, uint, 0 );
static int sec = 1;          // общее время работы
module_param( sec, uint, 0 );
static bool act = false;     // логический парамер
module_param( act, bool, 0 );

DEFINE_RT_MUTEX( lock );     // мютекс реального времени

typedef unsigned long parm_t;

static int thread_func( void *param ) { 
   unsigned long j1 = jiffies, j2; 
   while( !kthread_should_stop() ) {
      if( act ) rt_mutex_lock( &lock ); 
      ++*(parm_t*)param;     // выполняемая работа потоковой функции 
      msleep( 7 );
      if( act ) rt_mutex_unlock( &lock ); 
   }
   j2 = jiffies;
   LOG( "PID=%d : [%lX...%lX => %ld]", 
        current->pid, j1, j2, ( j2 - j1 ) * 1000 / HZ );
   return 0;
}

static int pa_init( void ) { 
   int i, ret = -1;
   parm_t *data = (parm_t*)vmalloc( thr * sizeof( parm_t ) ), // массив блоков данных
          sm = 0;
   struct task_struct **pt = (struct task_struct**)vmalloc( thr * sizeof( void* ) );
   static char s[ 120 ] = "rezults: ";
   if( NULL == data || NULL == pt ) {
      ret = -ENOMEM;
      goto end;
   }
   memset( data, 0, thr * sizeof( parm_t ) );                 // инициализация данных
   for( i = 0; i < thr; i++ )
      *( pt + i ) = kthread_run( thread_func, (void*)( data + i ), "pa_%d", i );
   msleep( 1000 * sec );                                      // интервал работы
   for( i = 0; i < thr; i++ )
      kthread_stop( *( pt + i ) );
   for( i = 0; i < thr; i++ ) {
      sm += *( data + i );
      sprintf( s + strlen( s ), "%ld%s",
               (long)*( data + i ), i != thr - 1 ? ", " : "" ); 
   }
   sprintf( s + strlen( s ), " => %ld\n", (long)sm );
   LOG( "%s", s );
end:  
   if( data != NULL ) vfree( data ); 
   if( pt != NULL ) vfree( pt ); 
   return ret; 
} 

module_init( pa_init ); 
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
