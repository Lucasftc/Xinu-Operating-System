/* wakeup.c - wakeup */

#include <xinu.h>
extern uint32 cycle_count;
/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */

	resched_cntl(DEFER_START);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		pid32 tempid;
		tempid=dequeue(sleepq);
		if(proctab[tempid].cyclestamp!=cycle_count){
			proctab[tempid].prprio = 40 - proctab[tempid].prprio;
			proctab[tempid].remaintime = 0;
		}
		ready(tempid);
	}

	resched_cntl(DEFER_STOP);
	return;
}
