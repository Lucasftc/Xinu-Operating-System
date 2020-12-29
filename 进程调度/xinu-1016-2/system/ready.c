/* ready.c - ready */

#include <xinu.h>

qid16 nextroundlist;
qid16 readylist; /* Index of ready list		*/
uint32 pexe=0;
uint32 cexe=0;
uint32 prem=0;
uint32 crem = 0;/*remain time in the list for the consumers and producters*/
/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{	
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	if(proctab[pid].prname[0]=='p')
		prem += proctab[pid].remaintime ? proctab[pid].remaintime : (uint32)proctab[pid].prprio;
	if(proctab[pid].prname[0]=='c')
		crem += proctab[pid].remaintime ? proctab[pid].remaintime : (uint32)proctab[pid].prprio;
	//if(isempty(readylist))
	//	resched();
	//else if (!isempty(readylist) && prptr->prprio > firstkey(readylist))
		resched();

	return OK;
}
