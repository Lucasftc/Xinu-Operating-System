/*  main.c  - main */

#include <xinu.h>
#define PRINTCNT 300

void sndA(void){
	int cnta=0;
	while (1)
	{
		int i;
		for (i = 0; i < 6000;i=i+1)
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
		for (i = 0; i < 6000;i=i+1)
			;
		putc(CONSOLE, 'B');
		//printf(" %d\n", i);
		cntb++;
		if(cntb==PRINTCNT)
			break;
	}
}
pid32 apid;
pid32 bpid;
process	main(void)
{
	//resume(apid = create(sndA, 1024, 20, "process 1", 0));
	//resume(bpid=create(sndB, 1024, 20, "process 2", 0));
	
}
