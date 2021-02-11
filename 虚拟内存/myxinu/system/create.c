/* create.c - create, newpid */

#include <xinu.h>

extern syscall add_table_to_dir(struct pd_t* pd, struct pt_t* pt,int offset);
extern syscall add_page_to_table(struct pt_t *pt, uint32 *addr, int offset);
extern uint32 get_free_frame();
extern syscall release_frame(uint32);
//extern const uint32* kpgdir;

local	int newpid();
/*------------------------------------------------------------------------
 *  create  -  Create a process to start running a function on x86
 *------------------------------------------------------------------------
 */
pid32	create(
	  void		*funcaddr,	/* Address of the function	*/
	  uint32	ssize,		/* Stack size in bytes		*/
	  pri16		priority,	/* Process priority > 0		*/
	  char		*name,		/* Name (for debugging)		*/
	  uint32	nargs,		/* Number of args that follow	*/
	  ...
	)
{
	uint32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/
	uint32      saddr_trans;

	mask = disable();
/************PAGE TABLE RELATED BEGIN***********/
	uint32 newdir_p=get_free_frame();
	uint32 curdir=(1023<<22)|(1023<<12)|0; //0xfffff000
	add_table_to_dir(curdir,newdir_p,1022);
	uint32 newdir_v=(1023<<22)|(1022<<12)|0;
	for(i=0;i<8;i++)
		add_table_to_dir(newdir_v,get_free_frame(),i);
	uint32 newtab_v=(1022<<22)|(0<<12)|0;
	init_core_pt(newtab_v,8);
	uint32 newstktab_p=get_free_frame();
	add_table_to_dir(newdir_v,newstktab_p,1021);
	add_table_to_dir(curdir,newstktab_p,1020);
	add_table_to_dir(newdir_v,newdir_p,1023);
	lcr3(proctab[currpid].pgdir);


/************PAGE TABLE RELATED END************/
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) PGROUNDUP(ssize);
	if ( (priority < 1) || ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)allocstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}
	//kprintf("after call allocstk\n");
	//kprintf("saddr: 0x%08X\n",saddr);
	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = priority;
	saddr_trans=(uint32) ((1021<<22)|(1023<<12)|((uint32)saddr&0xfff));
	prptr->prstkbase = (char *)saddr_trans;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	prptr->pgdir=newdir_p;
	
	/* Initialize stack as if the process was called*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/
	a += nargs -1;			/* Last argument		*/
	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process's stack	*/
	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registers, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	//*--saddr = savsp;		/* This will be register ebp	*/
	*--saddr=(1021<<22)|(1023<<12)|4092;
					/*   for process exit		*/
	savsp=(uint32) ((1021<<22)|((uint32) saddr & 0x3fffff));
	//savsp = (uint32) ((1021<<22)|(1023<<12)|((uint32)saddr&0xfff));		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/
	*--saddr = 0;			/* %eax */
	*--saddr = 0;			/* %ecx */
	*--saddr = 0;			/* %edx */
	*--saddr = 0;			/* %ebx */
	*--saddr = 0;			/* %esp; value filled in below	*/
	pushsp = saddr;			/* Remember this location	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
	*--saddr = 0;			/* %esi */
	*--saddr = 0;			/* %edi */
	saddr_trans=(uint32) ((1021<<22)|((uint32)saddr&0x3fffff));
	*pushsp = (unsigned long) (prptr->prstkptr = (uint32 *)saddr_trans);
	/************PAGE TABLE RELATED BEGIN************/
	//memset((uint32*)curdir+1020,0,4);//clear the temp directory entry and new process entry;
	//memset((uint32*)curdir+1022,0,4);
	//lcr3(kpgdir);
	/************PAGE TABLE RELATED END************/
	restore(mask);
	//kprintf("finish create\n");
	return pid;
	
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local	pid32	newpid(void)
{
	uint32	i;			/* Iterate through all processes*/
	static	pid32 nextpid = 1;	/* Position in table to try or	*/
					/*   one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* Wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}kprintf("newpid error\n");
	return (pid32) SYSERR;
}
