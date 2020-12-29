/*  main.c  - main */

#include <xinu.h>

/*
void test1(){
	int i, j,k,count=0;
	while(1){
		for (i = 0; i < 100000;i++)
			;
		putc(CONSOLE, 'A');
		count++;
		if(count==50)
			break;
	}
}

void test2(){
	int i, j,k,count=0;
	while(1){

		for (i = 0; i < 100000;i++)
			;

		putc(CONSOLE, 'B');
		count++;
		if(count==50)
			break;
	}
}
void test3(){
	int i, j,k,count=0;
	while(1){

		for (i = 0; i < 100000;i++)
			;

		putc(CONSOLE, 'C');
		count++;
		if(count==50)
			break;
	}
}
*/
int32 count1=0,count2=0,count3=0,count4=0,count5=0;
sid32 sim;
void test1(){
	int32 i;
	while (1)
	{
		for (i = 0; i < 10000;i++)
			;
		count1++;
		if(count1==6)
			sleepms(5);
	}
}
void test2(){
	int32 i;
	while(1){
		for (i = 0; i < 10000;i++)
			;
		count2++;
		if(count2==12)
			sleepms(5);
	}
}
void test3(){
	int32 i;
	while(1){
		for (i = 0; i < 10000;i++)
			;
		wait(sim);
		count3++;
		signal(sim);
	}
}
void test4(){
	int32 i;
	while(1){
		for (i = 0; i < 10000;i++)
			;
		wait(sim);
		count4++;
		signal(sim);
	}
}
void test5(){
	int32 i;
	while(1){
		for (i = 0; i < 10000;i++)
			;
		wait(sim);
		count5++;
		signal(sim);
	}
}
process	main(void)
{
	pid32 pid1, pid2,pid3,pid4,pid5;
	sim = semcreate(2);
	resched_cntl(DEFER_START);
	resume(pid1 = create(test1,1024,20,"test1",0));
	resume(pid2 = create(test1,1024,30,"test2",0));
	resume(pid3 = create(test1,1024,10,"test3",0));
	resume(pid4 = create(test1,1024,20,"test4",0));
	resume(pid5 = create(test1,1024,30,"test5",0));
	resched_cntl(DEFER_STOP);
	sleepms(200);
	kill(pid1);
	kill(pid2);
	kill(pid3);
	kill(pid4);
	kill(pid5);
	/*
	int32 count = 0;
	int32 sum = 0;
	resume(pid1 = create(test1, 1024, 10, "test1", 0));
	while(1){
		//kprintf("count1= %d count2=%d \n", count1, count2);
		count1 = 0;
		count2 = 0;
		sleep(0);
		count++;
		sum += count1;
		if (count >= 100)
		{
			kprintf("total:%d\n", sum);
			kill(pid1);
			//kill(pid2);
			break;
		}
	}
	*/
}
