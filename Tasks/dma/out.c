static int __init my_init( void );
module_init( my_init );

MODULE_AUTHOR( "Jerry Cooperstein" );
MODULE_AUTHOR( "Oleg Tsiliuric" );
MODULE_DESCRIPTION( "LDD:1.0 s_23/lab1_dma.c" );
MODULE_LICENSE( "GPL v2" );

#define MARK "=> "

static void output( char *kbuf, dma_addr_t handle, size_t size, char *string ) {
   unsigned long diff;
   diff = (unsigned long)kbuf - handle;
   printk( KERN_INFO MARK "kbuf=%12p, handle=%12p, size = %d\n",
           kbuf, (void*)(unsigned long)handle, (int)size );
   printk( KERN_INFO MARK "(kbuf-handle)= %12p, %12lu, PAGE_OFFSET=%12lu, compare=%lu\n",
           (void*)diff, diff, PAGE_OFFSET, diff - PAGE_OFFSET );
   strcpy( kbuf, string );
   printk( KERN_INFO MARK "string written was, %s\n", kbuf );
}


