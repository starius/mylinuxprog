#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/vmalloc.h>

static char* file = NULL;            // имя файла конфигураций
module_param( file, charp, 0 );
static bool debug = false;           // логический парамер
module_param( debug, bool, S_IRUGO );

#define LOG(...) if( debug ) printk( KERN_INFO "+ "__VA_ARGS__ )
#define INF(...) printk( KERN_INFO "+ "__VA_ARGS__ )
#define ERR(...) printk( KERN_ERR  "+ "__VA_ARGS__ )

//------------------ операции с циклическим буфером --------------------------------
//#define SIZE 1024 * 20
//#define SIZE 1024 * 3
#define SIZE 7
typedef struct {                     // блок циклического буфера
   struct list_head link;
   unsigned int     wpos, rpos;
   char             data[ SIZE ];
} data_t;

data_t* new( struct list_head *queue ) {        // новый блок в циклический буфер
   data_t *elem = (data_t*)vmalloc( sizeof( data_t ) );
   LOG( "allocate new block %p\n", elem );
   elem->wpos = elem->rpos = 0;
   list_add_tail( &elem->link, queue );
   return elem;
}

void erase( struct list_head *queue ) {  // удалить головгой блок из циклического буфера
   data_t *head;
   if( list_empty( queue ) ) return;
   head = list_entry( queue->next, data_t, link );
   LOG( "erase head block %p\n", head );
   list_del( queue->next );   
   vfree( head );
}

unsigned int len( struct list_head *queue  ) {   // число блоков в циклическом буфере
   unsigned int num = 0;
   struct list_head *iter;
   if( list_empty( queue ) ) return 0;
   list_for_each( iter, queue ) num++;
   return num;  
}

//----------------------- операции ввода-вывода ------------------------------------

static ssize_t conf_read( struct file *file, char *buf,     // чтение из /dev/...
                          size_t count, loff_t *ppos ) {
   struct list_head *queue = (struct list_head*)file->private_data; 
   data_t* rblock = list_entry( queue->next, data_t, link );
   ssize_t sumlen = 0;
   int free, len;
   if( list_empty( queue ) || rblock->rpos >= rblock->wpos ) {
      LOG( "read %u -> 0\n", (int)count );                  // EOF
      return 0;
   }
   do {
      rblock = list_entry( queue->next, data_t, link );
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
            erase( queue );
            if( list_empty( queue ) ) break;
         }
         else break;
      }
      if( count <= len ) break;
      count -= len;
   } while( count != 0 );
   return sumlen;
}

static ssize_t conf_write( struct file *file, const char *buf, // запись в /dev/...
                           size_t count, loff_t *ppos ) {
   struct list_head *queue = (struct list_head*)file->private_data ;
   data_t *wblock = list_empty( queue ) ? 
                    new( queue ) : list_entry( queue->prev, data_t, link );
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
      if( wblock->wpos >= SIZE ) 
         wblock = new( queue );
   } while( count != 0 );
   return sumlen;
}

LIST_HEAD( dev_list );          // цепочка каналов/устройств

typedef struct {                // блок описания канала
   struct list_head  link;      // список каналов
   struct list_head  queue;     // циклический буфер данных канала
   struct miscdevice chan; 
} chanel_t;

static int conf_open( struct inode *n, struct file *f ) {
   int minor = iminor( n );
   struct list_head *iter;
   chanel_t *c = NULL;
   f->private_data = NULL;
   list_for_each( iter, &dev_list ) { // связь с буфером канала через minor
      c = list_entry( iter, chanel_t, link );
      if( c->chan.minor == minor ) {
         f->private_data = &c->queue;
         break;
      }
   }
   LOG( "minor = %d\n", c->chan.minor );
   return 0;
}

//------------------------------ инициализации -------------------------------------

static const struct file_operations conf_fops = {
   .owner   = THIS_MODULE,   
   .open    = conf_open,     // открытие 
   .read    = conf_read,     // чтение из
   .write   = conf_write     // запись в
};

static int next_dev( char *name ) {  // новый эземпляр устройства
   chanel_t *elem = (chanel_t*)vmalloc( sizeof( chanel_t ) );
   int err;
   struct miscdevice template = {    // шаблон блока устройства
      .minor = MISC_DYNAMIC_MINOR,   // автоматически выбираемое
      .fops  = &conf_fops, 
      .mode  = 0666                  // флаги устройства
   };
   if( NULL == elem ) return -ENOMEM;
   elem->chan = template;
   elem->chan.name = (char*)vmalloc( strlen( name ) + 1 );
   if( NULL == elem->chan.name ) {
      vfree( elem );
      return -ENOMEM;
   }
   strcpy( (char*)( elem->chan.name ), name );
   err = misc_register( &elem->chan );
   if( err ) {
      vfree( elem->chan.name );
      vfree( elem );
      return err;
   }
   INIT_LIST_HEAD( &elem->queue  ); // пустой буфер данных
   list_add_tail( &elem->link, &dev_list );
   INF( "register device 10:%d\n", elem->chan.minor );
   return 0;
}

void erase_dev( void ) {            // удалить устройство в голове списка
   chanel_t *head;
   if( list_empty( &dev_list ) ) return;
   head = list_entry( dev_list.next, chanel_t, link );
   INF( "deregister device 10:%d\n", head->chan.minor );
//   while( !list_empty( head->queue ) ) 
//      erase( head->queue );
   list_del( dev_list.next );
   misc_deregister( &head->chan );
   vfree( head->chan.name );
   vfree( head );
}

#define BUF_LEN 2048
static int __init dev_init( void ) {
   int ret; //, i, nchan = 3;
   static char buff[ BUF_LEN + 1 ] = "/etc/confm.conf", // имя файла по умолчанию
              *b = &buff[ 0 ];
   struct file *f;
   size_t n;
   LOG( "data block size = %d\n", SIZE );
   if( file != NULL ) strcpy( buff, file );             // имя файла из команды загрузки
   LOG( "configuration file: %s\n", buff );
   f = filp_open( buff, O_RDONLY, 0 );
   if( IS_ERR( f ) ) {                                  // нет такого файла
      ERR( "illegal file: %s\n", buff );
      ret = -ENXIO;
      goto nofile;
   }
   n = kernel_read( f, 0, buff, BUF_LEN );              // читается весь файл целиком
   if( n < 0 ) {
      ERR( "read file\n" );
      ret = -EIO;
      goto clfile;
   }
   buff[ n ] = '\0';
   while( 1 ) {                                         // построчный разбор считанного
      char *e, *token;
      while( ' ' == *b ) b++;                           // убрать ведущие пробелы
      e = strchr( b, '\n' );
      if( NULL == e ) break;
      *e = '\0';
      if( '\0' == *b || '#' == *b ) {                   // комментарии и пустые строки
         b = e + 1;
         continue;
      } 
      token = strsep( &b, " \t=" );
      if( strcmp( token, "chan" ) != 0 ) {
         ERR( "illegal key: %s\n", token );
         ret = -EINVAL;
         goto clall;
      }
      do token = strsep( &b, " \t=" );
      while( b != NULL );
      INF( "device name: /dev/%s\n", token );           // зарегистрировать устройство
      ret = next_dev( token );
      if( ret ) {
         ERR( "unable to register channel /dev/%s\n", token );
         goto clall;
      }
      b = e + 1;
   }
   ret = 0;
   goto clfile;
clall:
   while( !list_empty( &dev_list ) ) erase_dev();
clfile:
   filp_close( f, NULL );
nofile:
   return ret;
}

static void __exit dev_exit( void ) {
   while( !list_empty( &dev_list ) ) erase_dev();
}

module_init( dev_init );
module_exit( dev_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_VERSION( "6.5" );

