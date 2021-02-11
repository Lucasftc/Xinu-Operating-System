/* allocmem.c - allocmem */

#include <xinu.h>
extern syscall add_table_to_dir(struct pd_t* pd, struct pt_t* pt,int offset);
extern syscall add_page_to_table(struct pt_t *pt, uint32 *addr, int offset);
extern uint32 get_free_frame();
extern syscall release_frame(uint32);
/*------------------------------------------------------------------------
 *  allocmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*allocmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	uint32 npages;
	intmask mask;
	uint32* memstart;
	struct pd_t* curdir=(struct pd_t*)((1023<<22)|(1023<<12)|0);
	int i,j;
	int startflag=1;
	nbytes=PGROUNDUP(nbytes);
	npages=nbytes/PGSIZE;
	mask=disable();
	if(npages==0){
		restore(mask);
		return SYSERR;
	}
	for(i=8;i<NPDENTRIES;i++){
		if(npages==0)
			break;
		if(curdir[i].pd_pres){
			struct pt_t* tab=(struct pt_t*)((1023<<22)|(i<<12)|0);
			for(j=0;j<NPTENTRIES;j++){
				if(npages==0)
					break;
				if(tab[j].pt_pres==0){
					if(startflag){
						memstart=(uint32*)((i<<22)|(j<<12)|0);
						startflag=0;
					}
					add_page_to_table(tab,get_free_frame(),j);
					npages--;
				}
			}
		}else{
			add_table_to_dir(curdir,get_free_frame(),i);
			i--;
			continue;
		}
	}
	restore(mask);
	return (char*)memstart;
}
