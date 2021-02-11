/* deallocmem.c - deallocmem */

#include <xinu.h>
extern syscall add_table_to_dir(struct pd_t* pd, struct pt_t* pt,int offset);
extern syscall add_page_to_table(struct pt_t *pt, uint32 *addr, int offset);
extern uint32 get_free_frame();
extern syscall release_frame(uint32);
/*------------------------------------------------------------------------
 *  deallocmem  -  Free a memory block, returning the block to the free list
 *------------------------------------------------------------------------
 */
syscall	deallocmem(
	  char		*addr,	/* Pointer to memory block	*/
	  uint32	nbytes		/* Size of block in bytes	*/
	)
{
	intmask mask;
	int i,j;
	struct pd_t* curdir=(struct pd_t*)((1023<<22)|(1023<<12)|0);
	int dir_index=(uint32)addr>>22;
	int tab_index=((uint32)addr>>12) & 0x3ff;
	nbytes=PGROUNDUP(nbytes);
	int npages=nbytes/PGSIZE;
	mask = disable();
	if ((nbytes == 0)) {
		restore(mask);
		return SYSERR;
	}
	for(i=dir_index;i<NPDENTRIES;i++){
		if(npages==0)
			break;
		struct pt_t* tab=(struct pt_t*)((1023<<22)|(i<<12)|0);
		for(j=tab_index;j<NPTENTRIES;j++){
			if(npages==0)
				break;
			if(release_frame(tab[j].pt_base<<PTXSHIFT)==SYSERR)
				panic("can't find the frame to release\n");
			memset(tab+j,0,4);
			npages--;
		}
		tab_index=0;
	}
	restore(mask);
	return OK;
}
