/* kill.c - kill */

#include <xinu.h>
extern syscall add_table_to_dir(struct pd_t* pd, struct pt_t* pt,int offset);
extern syscall add_page_to_table(struct pt_t *pt, uint32 *addr, int offset);
extern uint32 get_free_frame();
extern syscall release_frame(uint32);
/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */

syscall deallocvm(char* addr,uint32 nbytes){
	struct pd_t* curdir=(struct pd_t*)((1023<<22)|(1023<<12)|0);
	memcpy((void*)0x0,(void*)curdir,PGSIZE);
	struct pt_t*stktab=(struct pt_t*)((1023<<22)|(1021<<12)|0);
	memcpy((void*)0x1000,(void*)stktab,PGSIZE);
	add_table_to_dir((struct pd_t*)0x0,(struct pt_t*)0x1000,1021);
	
	int i,j;
	for(i=8;i<NPDENTRIES;i++){
		if(curdir[i].pd_pres==0)
			break;
		struct pt_t* tab=(struct pt_t*)((1023<<22)|(i<<12)|0);
		for(j=0;j<NPTENTRIES;j++){
			if(tab[j].pt_pres==0)
				break;
			if(release_frame(tab[j].pt_base<<PTXSHIFT)==SYSERR){
				kprintf("1: can't find frame 0x%08X when release in deallocstk\n",tab[j].pt_base<<PTXSHIFT);
				panic("");
			}
			memset(tab+j,0,4);
		}
		if(release_frame(curdir[i].pd_base<<PTXSHIFT)==SYSERR){
			kprintf("2: can't find frame 0x%08X when release in deallocstk\n",curdir[i].pd_base<<PTXSHIFT);
			panic("");
		}
			
		memset(curdir+i,0,4);
	}

	
	for(i=NPTENTRIES-1;i>=0;i--){
		if(stktab[i].pt_pres==0)
			break;
		if(release_frame(stktab[i].pt_base<<PTXSHIFT)==SYSERR){
			kprintf("3: can't find frame 0x%08X when release in deallocstk\n",stktab[i].pt_base<<PTXSHIFT);
			panic("");
		}
		memset(stktab+i,0,4);
	}
	

	release_frame(proctab[getpid()].pgdir);
	return OK;

}
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/

	//kprintf("kill %s\n",proctab[pid].prname);
	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

	deallocvm((uint32*)(prptr->prstkbase), prptr->prstklen);
	//kprintf("finish deallocvm\n");
	lcr3(0);




	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}
	
	/*
	struct pt_t* tmpstktab=(struct pt_t*)0x100;
	for(i=NPTENTRIES-1;i>=0;i--){
		if(tmpstktab[i].pt_pres==0)
			break;
		if(release_frame(tmpstktab[i].pt_base<<PTXSHIFT)==SYSERR){
			kprintf("3: can't find frame 0x%08X when release in deallocstk\n",tmpstktab[i].pt_base<<PTXSHIFT);
			panic("");
		}
		memset(tmpstktab+i,0,4);
	}
	*/

	restore(mask);
	return OK;
}
