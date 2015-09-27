#include <linux/list.h>
#include <linux/vmalloc.h>
#include "common.c"

//#define SIZE 1024 * 20
//#define SIZE 1024 * 3
#define SIZE 7
typedef struct {             // блок циклического буфера
   struct list_head link;
   unsigned int     wpos, rpos;
   char             data[ SIZE ];
} data_t;

LIST_HEAD( queue );

data_t* new( void ) {        // новый блок в циклический буфер
   data_t *elem = (data_t*)vmalloc( sizeof( data_t ) );
   LOG( "allocate new block %p\n", elem );
   elem->wpos = elem->rpos = 0;
   list_add_tail( &elem->link, &queue );
   return elem;
}

void erase( void ) {         // удалить отработанный блок из циклического буфера
   data_t *head;
   if( list_empty( &queue ) ) return;
   head = list_entry( queue.next, data_t, link );
   LOG( "erase head block %p\n", head );
   list_del( queue.next );   
   vfree( head );
}

unsigned int len( void ) {   // число блоков в циклическом буфере
   unsigned int num = 0;
   struct list_head *iter;
   if( list_empty( &queue ) ) return 0;
   list_for_each( iter, &queue ) num++;
   return num;  
}

//----------------------------------------------------------------------------------

static ssize_t queue_read( struct file *file, char *buf,     // чтение из /dev/queue 
                           size_t count, loff_t *ppos ) {
   data_t* rblock = list_entry( queue.next, data_t, link );
   ssize_t sumlen = 0;
   int free, len;
   if( list_empty( &queue ) || rblock->rpos >= rblock->wpos ) {
      LOG( "read %u -> 0\n", (int)count );                  // EOF
      return 0;
   }
   do {
      rblock = list_entry( queue.next, data_t, link );
      free = rblock->wpos - rblock->rpos;
      len = count < free ? count : free;
      if( copy_to_user( buf, rblock->data + rblock->rpos, len ) )
         return -EINVAL;
      LOG( "read %u -> %u\n", (int)count, len );
      sumlen += len;
      *ppos += len;
      rblock->rpos += len;
      buf += len;
      if( rblock->rpos >= rblock->wpos ) {
         if( rblock->rpos >= SIZE ) {
            erase();
            if( list_empty( &queue ) ) break;
         }
         else break;
      }
      if( count <= len ) break;
      count -= len;
   } while( count != 0 );
   return sumlen;
}

static ssize_t queue_write( struct file *f, const char *buf, // запись в /dev/queue
                            size_t count, loff_t *ppos ) {
   data_t *wblock = list_empty( &queue ) ? new() : list_entry( queue.prev, data_t, link );
   ssize_t sumlen = 0;
   int free, len;
   do {
      free = SIZE - wblock->wpos;
      len = count < free ? count : free;
      if( copy_from_user( wblock->data + wblock->wpos, buf, len ) )
         return -EINVAL;
      LOG( "write %u -> %u\n", (int)count, len );
      count -= len;
      sumlen += len;
      wblock->wpos += len;
      *ppos += len;
      buf += len;
      if( wblock->wpos >= SIZE ) {
         wblock = new();
      };
   } while( count != 0 );
   return sumlen;
}

static int __init dev_init( void ) {
   int ret;
   if( minor != 0 ) queue_dev.minor = minor;
   ret = misc_register( &queue_dev );
   if( ret ) {
      printk( KERN_ERR "=== unable to register misc device\n" );
      goto end;
   }
   printk( KERN_INFO "=== register device 10:%d\n", queue_dev.minor );
   LOG( "block size = %d\n", SIZE );
end:
   return ret;
}

static void __exit dev_exit( void ) {
   while( !list_empty( &queue ) ) erase();
   printk( KERN_INFO "=== deregister device 10:%d\n", queue_dev.minor );
   misc_deregister( &queue_dev );
}
