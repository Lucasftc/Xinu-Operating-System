/*  main.c  - main */

#include <xinu.h>
//extern char* allocmem(uint32);
//extern syscall deallocmem(char*,uint32);

void test1(){
	int i,j,k;
	kprintf("test1: 0x%08X\n",&k);
}
void test2(){
	int i,j,k;
	kprintf("test2: 0x%08X\n",&k);
}
void testfunc(){
	return;
}
process	main(void)
{
	kprintf("enter main\n");
	/*
	char* m1=allocmem(4096);
	char* m2=allocmem(4*1024*1024);
	char* m3=allocmem(8192);
	char* m4=allocmem(4096);
	kprintf("allocmem:\nm1: 0x%08X\nm2: 0x%08X\nm3: 0x%08X\nm4: 0x%08X\n",m1,m2,m3,m4);
	deallocmem(m1,4096);
	deallocmem(m2,4*1024*1024);
	deallocmem(m3,8192);
	deallocmem(m4,4096);
	*/



	/*
	resume(create(test1,1024,20,"process1",0));
	resume(create(test2,1024,20,"process2",0));
	*/
	/* Run the Xinu shell */
	
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	// Wait for shell to exit and recreate it 
	while (TRUE)
	{
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
	
}

