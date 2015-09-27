/* **************** LDD:1.0 s_23/lab1_dma.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
* DMA Memory Allocation (new API solution)
*
* Write a module that allocates and maps a suitable DMA buffer, and
* obtains the bus address handle.
*
* Do this in three ways:
*    Using dma_alloc_coherent().
*    Using dma_map_single()
*    Using a DMA Pool.
*
* You can use NULL for the device and/or pci_dev structure arguments
* since we don't actually have a physical device.
*
* Compare the resulting kernel and bus addresses; how do they differ?
* Compare with the value of PAGE_OFFSET.
*
* In each case copy a string into the buffer and make sure it can be
* read back properly.
*
* In the case of dma_map_single(), you may want to compare the use of
* different direction arguments.
*
* We give two solutions, one with the new bus-independent interface,
* and one with the older PCI API.
*
@*/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

#include "out.c"

#define pool_size 1024
#define pool_align 8

// int direction = PCI_DMA_TODEVICE ;
// int direction = PCI_DMA_FROMDEVICE ;
static int direction = PCI_DMA_BIDIRECTIONAL;
//int direction = PCI_DMA_NONE;

static int __init my_init( void ) {
   char *kbuf;
   dma_addr_t handle;
   size_t size = ( 10 * PAGE_SIZE );
   struct dma_pool *mypool;
   /* dma_alloc_coherent method */
   kbuf = dma_alloc_coherent( NULL, size, &handle, GFP_KERNEL );
   output( kbuf, handle, size, "This is the dma_alloc_coherent() string" );
   dma_free_coherent( NULL, size, kbuf, handle );
   /* dma_map/unmap_single */
   kbuf = kmalloc( size, GFP_KERNEL );
   handle = dma_map_single( NULL, kbuf, size, direction );
   output( kbuf, handle, size, "This is the dma_map_single() string" );
   dma_unmap_single( NULL, handle, size, direction );
   kfree( kbuf );
   /* dma_pool method */
   mypool = dma_pool_create( "mypool", NULL, pool_size, pool_align, 0 );
   kbuf = dma_pool_alloc( mypool, GFP_KERNEL, &handle );
   output( kbuf, handle, size, "This is the dma_pool_alloc() string" );
   dma_pool_free( mypool, kbuf, handle );
   dma_pool_destroy( mypool );
   return -1;
}
