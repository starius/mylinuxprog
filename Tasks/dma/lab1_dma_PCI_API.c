/* **************** LDD:1.0 s_23/lab1_dma_PCI_API.c **************** */
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
 * DMA Memory Allocation  (old PCI API solution)
 *
 * Write a module that allocates and maps a suitable DMA buffer, and
 * obtains the bus address handle.
 *
 * Do this in two ways; first using dma_alloc_coherent() and then
 * using dma_map_single().  You can use NULL for the device and/or
 * pci_dev structure arguments since we don't actually have a physical
 * device.
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
 @*/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include "out.c"

// int direction = PCI_DMA_TODEVICE ;
// int direction = PCI_DMA_FROMDEVICE ;
static int direction = PCI_DMA_BIDIRECTIONAL;
//int direction = PCI_DMA_NONE;

static int __init my_init( void ) {
   char *kbuf;
   dma_addr_t handle;
   size_t size = ( 10 * PAGE_SIZE );
   /* pci_alloc_consistent method */
   kbuf = pci_alloc_consistent( NULL, size, &handle );
   output( kbuf, handle, size, "This is the pci_alloc_consistent() string" );
   pci_free_consistent( NULL, size, kbuf, handle );
   /* pci_map/unmap_single */
   kbuf = kmalloc( size, GFP_KERNEL );
   handle = pci_map_single( NULL, kbuf, size, direction );
   output( kbuf, handle, size, "This is the pci_map_single() string" );
   pci_unmap_single( NULL, handle, size, direction );
   kfree( kbuf );
   return -1;
}
