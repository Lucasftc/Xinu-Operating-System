/*  main.c  - main */

#include <xinu.h>
#define PRINTCNT 100
pid32 apid;
pid32 bpid;
void sndA(void){
	int cnta=0;
	while (1)
	{
		int i;
		for (i = 0; i < 1000;i=i+1)
			;
		putc(CONSOLE, 'A');
		//printf(" %d\n", i);
		cnta++;
		if(cnta==PRINTCNT)
			break;
	}
}
void sndB(void){
	int cntb=0;
	while(1){
		int i;
		for (i = 0; i < 1000;i=i+1)
			;
		putc(CONSOLE, 'B');
		//printf(" %d\n", i);
		cntb++;
		if(cntb==PRINTCNT)
			break;
	}
}

process	main(void)
{

	/* Run the Xinu shell */

	//recvclr();
	//resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */
	/*
	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    */
	resume(apid = create(sndA, 1024, 20, "process 1", 0));
	resume(bpid=create(sndB, 1024, 20, "process 2", 0));
	
}
