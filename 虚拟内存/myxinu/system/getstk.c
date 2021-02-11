/* allocstk.c - allocstk */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  allocstk  -  Allocate stack memory, returning highest word address
 *------------------------------------------------------------------------
 */
char  	*allocstk(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	int i;
	uint32 npages;
	intmask	mask;			/* Saved interrupt mask		*/
	//struct	memblk	*prev, *curr;	/* Walk through memory list	*/
	//struct	memblk	*fits, *fitsprev; /* Record block that fits	*/

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) PGROUNDUP(nbytes);	/* Use mblock multiples	*/
	npages=nbytes/PGSIZE;
	uint32 stktab=(1022<<22)|(1021<<12)|0;
	for(i=0;i<npages;i++){
		add_page_to_table(stktab,get_free_frame(),1023-i);
	}
	restore(mask);
	//if(getpid()==NULLPROC)
	//	return (char*)(((1021<<22)|(1023<<12)|4096)-sizeof(uint32));
	//return (char*)(((1020<<22)|(1023<<12)|4096)-sizeof(uint32));
	return (char*) ((1020<<22)|(1023<<12)|(4096-sizeof(uint32)));
	
}
