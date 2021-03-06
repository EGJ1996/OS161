/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ADDRSPACE_H_
#define _ADDRSPACE_H_

/*
 * Address space structure and operations.
 */


#include <vm.h>
#include "opt-dumbvm.h"

struct vnode;
struct cpu;
/**
 * Added by Pratham Malik for ASST3 - VM
 */

//Declaring the same number of stack pages as now of now -- might change later
#define VM_STACKPAGES    12

struct page_table_entry
{
	paddr_t pa;			//Stores the physical address to which page is mapped
	vaddr_t va;			//Stores the virtual address to which page is mapped
	unsigned int permissions;	//Stores the permissions for the page table entry

	/**
	 * present == 1 -- Entry present in coremap
	 * present == 0 -- Entry present in the disk
	 */

	unsigned int present:2;		//Variable for checking whether the page is in physical memory or disk

	//Store the index in the swap file when the page is being swapped out
	int swapfile_index;

	struct page_table_entry *next;
};

//Define Regions
struct addr_regions
{

	vaddr_t va_start;			//Virtual address of the start of region
	vaddr_t va_end;				//Virtual address of the end of region

	int region_numpages;		//Number of pages assigned for the region

	//Permissions -- Set to 1 if permission given and 0 if permission not given

	int set_permissions;		//Used for saving the old permissions - to be retrieved using bit manipulation
	int new_permission;			//TODO: Used for setting the new permissions - check this and change this to set_permission in as_complete_load

	struct addr_regions *next_region;		//Link to the next region as we don't know the number of regions

};


//End of additions of structures and variables and functions by PM

/*
 * Address space - data structure associated with the virtual memory
 * space of a process.
 *
 * You write this.
 */

struct addrspace {
#if OPT_DUMBVM
        vaddr_t as_vbase1;
        paddr_t as_pbase1;
        size_t as_npages1;

        vaddr_t as_vbase2;
        paddr_t as_pbase2;
        size_t as_npages2;

        paddr_t as_stackpbase;
#else
        /* Put stuff here for your VM system */

        struct page_table_entry *page_table;
        vaddr_t heap_start;
        vaddr_t heap_end;
        vaddr_t stackbase_top;		//Should store USERSTACK LOCATION
        vaddr_t stackbase_base;

        struct addr_regions *regions;		//Link list of all the regions
        struct lock *lock_page_table;		//Lock for accessing the page table


//
        struct page_table_entry *pagelist_head;
        int num_pages;
        #endif
};

/*
 * Functions in addrspace.c:
 *
 *    as_create - create a new empty address space. You need to make 
 *                sure this gets called in all the right places. You
 *                may find you want to change the argument list. May
 *                return NULL on out-of-memory error.
 *
 *    as_copy   - create a new address space that is an exact copy of
 *                an old one. Probably calls as_create to get a new
 *                empty address space and fill it in, but that's up to
 *                you.
 *
 *    as_activate - make the specified address space the one currently
 *                "seen" by the processor. Argument might be NULL, 
 *                meaning "no particular address space".
 *
 *    as_destroy - dispose of an address space. You may need to change
 *                the way this works if implementing user-level threads.
 *
 *    as_define_region - set up a region of memory within the address
 *                space.
 *
 *    as_prepare_load - this is called before actually loading from an
 *                executable into the address space.
 *
 *    as_complete_load - this is called when loading from an executable
 *                is complete.
 *
 *    as_define_stack - set up the stack region in the address space.
 *                (Normally called *after* as_complete_load().) Hands
 *                back the initial stack pointer for the new process.
 */

struct addrspace *as_create(void);
int               as_copy(struct addrspace *src, struct addrspace **ret);
void              as_activate(struct addrspace *);
void              as_destroy(struct addrspace *);

int               as_define_region(struct addrspace *as, 
                                   vaddr_t vaddr, size_t sz,
                                   int readable, 
                                   int writeable,
                                   int executable);
int               as_prepare_load(struct addrspace *as);
int               as_complete_load(struct addrspace *as);
int               as_define_stack(struct addrspace *as, vaddr_t *initstackptr);
void			  as_zero_region(paddr_t paddr, unsigned npages);

/*paddr_t alloc_newPage(struct addrspace *new,int *index,struct addrspace *old);*/
paddr_t alloc_newPage(struct addrspace *new);




/*
 * Functions in loadelf.c
 *    load_elf - load an ELF user program executable into the current
 *               address space. Returns the entry point (initial PC)
 *               in the space pointed to by ENTRYPOINT.
 */

int load_elf(struct vnode *v, vaddr_t *entrypoint);

void
write_page(paddr_t pa, int index);

void
read_page(paddr_t pa, int index);

int
make_swap_file(void);

/*
struct page_table_entry*
pagetable_copy(struct page_table_entry *oldpage,struct addrspace *as);
*/

void
fix_old_pages(struct addrspace *old);

void
free_old_pages(struct addrspace *old);


int
find_index(paddr_t pa);



#endif /* _ADDRSPACE_H_ */
