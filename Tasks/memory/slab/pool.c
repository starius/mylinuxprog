#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mempool.h>
#include <linux/version.h>
#include <linux/timex.h>
#include <linux/sched.h>

static long size = 7;  // для наглядности - простые числа 
module_param( size, long, 0 );
static long number = 31;
module_param( number, long, 0 );
static int mode = 0;   // 0-slab, 1-kmalloc
module_param( mode, int, S_IRUGO );

#define SLABNAME "my_cache"
static struct kmem_cache *cache = NULL; 
static mempool_t *pool = NULL;  
#define POOL_MINIMUM 11

mempool_t *pool_create( void ) {
   switch( mode ) {
      case 0:
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
         cache = kmem_cache_create( SLABNAME, size, 0, SLAB_HWCACHE_ALIGN, NULL, NULL );
#else
         cache = kmem_cache_create( SLABNAME, size, 0, SLAB_HWCACHE_ALIGN, NULL );
#endif
         if( !cache ) {
            printk( KERN_ERR "cache create error\n" );
            return NULL;
         }
         return mempool_create_slab_pool( POOL_MINIMUM, cache );
      case 1:
         return mempool_create_kmalloc_pool( POOL_MINIMUM, size );
   }
   return NULL;
}

void pool_destroy( void ) {
   if( pool != NULL ) mempool_destroy( pool );
   if( 0 == mode && cache != NULL ) kmem_cache_destroy( cache );
}

void *obj_alloc( void ) {
   switch( mode ) {
      case 0:
         return mempool_alloc_slab( GFP_KERNEL, cache );
      case 1:
         return mempool_kmalloc( GFP_KERNEL, (void*)size );
   }
   return NULL;
};

int obj_test( void *obj, int ind ) {
   int i;
   for( i = 0; i < size; i++ ) *(char*)obj = ( ind + i ) % 256;
   return 1;
};

void obj_free( void *obj ) {
   switch( mode ) {
      case 0:
         mempool_free_slab( obj, cache );
         return;
      case 1:
         mempool_kfree( obj, (void*)size );
         return;
   }
};

static void* *line = NULL;

static int __init init( void ) {
   int i;
   long tst = 0, sum = 0;
   if( mode != 0 && mode != 1 ) {
      printk( KERN_ERR "illegal mode value\n" );
      return -EINVAL;
   }
   if( !( line = kmalloc( sizeof(void*) * number, GFP_KERNEL ) ) ) {
      printk( KERN_ERR "kmalloc error\n" );
      goto mout;
   }
   for( i = 0; i < number; i++ ) line[ i ] = NULL; 
   if( !( pool = pool_create() ) ) {
      printk( KERN_ERR "pool create error\n" );
      goto pout;
   };
   printk( KERN_INFO "pool created\n" );
   printk( KERN_INFO "%d objects in pool before allocation\n", pool->curr_nr );
   for( i = 0; i < number; i++ ) {
      cycles_t t1, t2;
      schedule();   
      t1 = get_cycles();
      if( !( line[ i ] = obj_alloc() ) ) {
         printk( KERN_ERR "object allocation error\n" );
         goto oout;
      }
      t2 = get_cycles();
      sum += ( t2 - t1 ); 
   }
   sum /= number;
   for( i = 0; i < number; i++ ) tst += ( obj_test( line[ i ], i ) ? 1 : 0 );
   printk( KERN_INFO "%d objects in pool after allocation\n", pool->curr_nr );
   printk( KERN_INFO "allocate %ld objects (size %ld) in %ld cycles\n", number, size, sum );
   printk( KERN_INFO "tested OK objects: %ld\n", tst );
   for( i = 0; i < number; i++ ) obj_free( line[ i ] ); 
   pool_destroy();
   printk( KERN_INFO "pool destroyed\n" );
   kfree( line );
   return -1; //0;
oout:
   for( i = 0; i < number; i++ ) obj_free( line[ i ] ); 
pout:
   pool_destroy();
mout:
   kfree( line );
   return -ENOMEM;
}
module_init( init );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_VERSION( "5.2" );


/*
typedef struct mempool_s {
        spinlock_t lock;
        int min_nr;             // nr of elements at *elements 
        int curr_nr;            // Current nr of elements at *elements 
        void **elements;

        void *pool_data;
        mempool_alloc_t *alloc;
        mempool_free_t *free;
        wait_queue_head_t wait;
} mempool_t;
*/
