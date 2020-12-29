/* resched.c - resched, resched_cntl */
#include <xinu.h>

struct	defer	Defer;
uint32 cycle_count;

extern uint32 prem;
extern uint32 crem;
extern uint32 cexe;
extern uint32 pexe;
/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		//if (ptold->prprio > firstkey(readylist)) {
		//	return;
		//}
		/* Old process will no longer remain current */
		// PR_READY means the process was scheduled for time slice 
		ptold->prstate = PR_READY;
		insert(currpid, nextroundlist, ptold->prprio);
		//insert(currpid, readylist, ptold->prprio);
	}
	if(ptold->prname[0]=='p')
			pexe += ptold->prprio - preempt;
		else if(ptold->prname[0]=='c')
			cexe += ptold->prprio - preempt;
		else if(ptold->prname[0]=='s' && ptold->prname[1]=='w'){
			if(ptold->prmsg==1)
				pexe+=ptold->prprio - preempt;
			else if(ptold->prmsg==2)
				cexe += ptold->prprio - preempt;
		}
	/* Force context switch to highest priority ready process */
	if(isempty(readylist)){
		kprintf("round end \n");
		cycle_count++;
		cycle_count %= 100;
		prem = crem = pexe = cexe = 0;
		while (!isempty(nextroundlist))
		{
			pid32 tempid = dequeue(nextroundlist);
			proctab[tempid].remaintime = 0;
			proctab[tempid].prprio = 40 - proctab[tempid].prprio;
			if(proctab[tempid].prname[0]=='p')
				prem += proctab[tempid].prprio;
			else if(proctab[tempid].prname[0]=='c')
				crem += proctab[tempid].prprio;
			insert(tempid, readylist, proctab[tempid].prprio);
		}
		currpid = dequeue(readylist);

	}else{
		currpid = dequeue(readylist);
		if(currpid==NULLPROC){
			kprintf("round end \n");
			cycle_count++;
			cycle_count %= 100;
			prem = crem = pexe = cexe = 0;
			while (!isempty(nextroundlist))
			{
				pid32 tempid = dequeue(nextroundlist);
				proctab[tempid].remaintime = 0;
				proctab[tempid].prprio = 40 - proctab[tempid].prprio;
				if(proctab[tempid].prname[0]=='p')
					prem += proctab[tempid].prprio;
				else if(proctab[tempid].prname[0]=='c')
					crem += proctab[tempid].prprio;
				insert(tempid, readylist, proctab[tempid].prprio);
			}
			insert(NULLPROC, readylist, 0);
			currpid = dequeue(readylist);
		}
	}
	//reset preempt according to their priority
	kprintf("cexe:%u crem:%u  pexe:%u prem:%u     ", cexe, crem, pexe, prem);
	kprintf("choose:%s\n", proctab[currpid].prname);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	if(ptnew->remaintime!=0){
		preempt = ptnew->remaintime;
		ptnew->remaintime = 0;
	}else{
		if (ptnew->prprio == PRIO1)
			preempt = QUANTUM1;
		else if (ptnew->prprio == PRIO2)
			preempt = QUANTUM2;
		else if (ptnew->prprio == PRIO3)
			preempt = QUANTUM3;
		else
			preempt = QUANTUM3;
	}
	if(ptnew->prname[0]=='p')
		prem -= preempt;
	if(ptnew->prname[0]=='c')
		crem -= preempt;
	if (ptnew->prname[0] == 's' && ptnew->prname[1] == 'w')
	{
		if(prem+pexe<=crem+cexe)
			send(currpid, 1);
		else
			send(currpid, 2);
	}
	//preempt = QUANTUM; /* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	/* Old process returns here when resumed */
	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {
	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}