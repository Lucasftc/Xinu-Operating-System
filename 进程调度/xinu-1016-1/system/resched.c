/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	static uint32 exetime[5]={0};
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

		ptold->prstate = PR_READY;
		insert(currpid, nextroundlist, ptold->prprio);
		//insert(currpid, readylist, ptold->prprio);
	}
	if(ptold->prname[4]>='0' && ptold->prname[4]<='9'){
		int32 i = ptold->prname[4] - '1';
		exetime[i] += ptold->prprio - preempt;
	}
	/* Force context switch to highest priority ready process */
	if(isempty(readylist)){
		while(!isempty(nextroundlist)){
			pid32 tempid = dequeue(nextroundlist);
			insert(tempid, readylist, proctab[tempid].prprio);
		}
		currpid = dequeue(readylist);
	}else{
		currpid = dequeue(readylist);
		if(currpid==NULLPROC){
			while(!isempty(nextroundlist)){
				pid32 tempid = dequeue(nextroundlist);
				insert(tempid, readylist, proctab[tempid].prprio);
			}
			insert(NULLPROC, readylist, 0);
			currpid = dequeue(readylist);
		}
	}
	kprintf("test1: %u test2: %u test3: %u test4: %u test5: %u \n", exetime[0], exetime[1], exetime[2], exetime[3], exetime[4]);
	//reset preempt according to their priority
	//kprintf("%s\n", proctab[currpid].prname);
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
