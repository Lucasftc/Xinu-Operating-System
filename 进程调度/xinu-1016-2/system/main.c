/*  main.c  - main */

#include <xinu.h>

#define N 100
int32 buffer[N];
int32 readindex;//the latest index that has been read
int32 writeindex;// the last index that has been written
sid32 psem, csem, pmux, cmux;
//for testing a slice's workload
int32 cnt;
//for dubugging
extern uint32 pexe, prem, crem, cexe;
void test1()
{
	int i;//test how many slots can producer write:
	while (1){//if i==50000,each 10ms slice the producer will write 1 slot
			  //if i==10000,     10ms                               5.75
		for (i = 0; i < 10000;i++)
			;
		writeindex++;
		if(writeindex>=N)
			writeindex %= N;
		buffer[writeindex] = 1;
		cnt++;
	}
}

void test2(){
	int i=0;
	int data; //test how many slots can consumer read:
			  //the same as the producer;
	while (1)
	{
		for (i = 0; i < 50000;i++)
			;
		readindex++;
		if(readindex>=N)
			readindex %= N;
		data = buffer[readindex];
		cnt++;
	}
}

void test0(){
	int i, j,k,count=0;
	while(1){

		for (i = 0; i < 10000;i++)
			;

		putc(CONSOLE, 'A');
		count++;
		if(count==50)
			break;
	}
}
void consumer(){
	int32 data;
	int32 i;
	int32 count = 0;
	while (1)
	{
		for (i = 0; i < 10000; i++)
			;
		wait(csem);
		wait(cmux);
		readindex++;
		if(readindex>=N)
			readindex %= N;
		data = buffer[readindex];
		signal(cmux);
		signal(psem);
		count++;
		if(count==5){
			count = 0;	
			sleepms(5); //only if consumer's priority > producer;
		}
	}
}
void producer(){
	int32 i;
	int32 count = 0;
	while (1)
	{
		for (i = 0; i < 10000;i++)
			;
		wait(psem);
		wait(pmux);
		writeindex++;
		if(writeindex>=N)
			writeindex %= N;
		buffer[writeindex] = 1;
		signal(pmux);
		signal(csem);
		count++;
		if(count==5){
			count = 0;
			sleepms(5);//only if producer's priority > consumer
		}
	}
}
void swayer(){
	int32 i;
	int32 count = 0;
	while (1)
	{
		umsg32 msg = receive();
		/*umsg32 msg;
		if(pexe+prem<=crem+cexe)
			msg = 1;
		else
			msg = 2;
		*/
		if (msg == 1)
		{
			while(1){
				for (i = 0; i < 10000;i++)
					;
				wait(psem);
				wait(pmux);
				writeindex++;
				if(writeindex>=N)
					writeindex %= N;
				buffer[writeindex] = 2;
				signal(pmux);
				signal(csem);
				count++;
				if(count==5){
					count = 0;
					break;
				}
			}
		}
		else if(msg==2){
			int32 data;
			while (1)
			{
				for (i = 0; i < 10000;i++)
					;
				wait(csem);
				wait(cmux);
				readindex++;
				if(readindex>=N)
					readindex%=N;
				data = buffer[readindex];
				signal(cmux);
				signal(psem);
				count++;
				if(count==5){
					count = 0;
					break;
				}
			}
		}
	}
}

process	main(void)
{
	pid32 ppid1,ppid2,ppid3, cpid1,cpid2,cpid3,spid;
	pmux = semcreate(1);
	cmux = semcreate(1);
	psem = semcreate(N);
	csem = semcreate(0);
	readindex = -1;
	writeindex = -1;
	resched_cntl(DEFER_START);
	resume(ppid1=create(producer, 1024, 30, "producer", 0));
	resume(ppid2=create(producer, 1024, 30, "producer", 0));
	resume(ppid3=create(producer, 1024, 30, "producer", 0));
	resume(spid=create(swayer, 1024, 20, "swayer", 0));
	resume(cpid1=create(consumer, 1024, 10, "consumer", 0));
	resume(cpid2=create(consumer, 1024, 10, "consumer", 0));
	resume(cpid3=create(consumer, 1024, 10, "consumer", 0));
	resched_cntl(DEFER_STOP);
	sleepms(800);
	kill(ppid1);
	kill(ppid2);
	kill(ppid3);
	kill(spid);
	kill(cpid1);
	kill(cpid2);
	kill(cpid3);
	//code for test a time slice 's work load
	/*
	int cnt_sum = 0;
	cnt = readindex =writeindex= 0;
	int i,j=0;
	for (i = 0; i < N;i++)
		buffer[i] = 0;
	pid32 test1pid;
	resume(test1pid=create(test2, 1024, 10, "test1", 0));
	while(1){
		cnt_sum += cnt;
		cnt = 0;
		j++;
		if(j==100){
			kill(test1pid);
			break;
		}
		resched();
	}
	kprintf("avg: %d", cnt_sum);
	*/
	//resume(create(test2, 1024, 30, "test2", 0));
	//resume(create(test3, 1024, 30, "test3", 0));
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
}

