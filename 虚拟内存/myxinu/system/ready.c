/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	//kprintf("enter ready\n");
	register struct procent *prptr;
	if (isbadpid(pid)) {
		return SYSERR;
	}
	/* Set process state to indicate ready and add to ready list */
	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	if(insert(pid, readylist, prptr->prprio))
		//kprintf("finish insert,prio is %u\n",prptr->prprio);
	resched();
	return OK;
}