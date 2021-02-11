#include "xinu.h"
#include "kbd.h"
extern void set_cursor(uint16 pos);
void kbdhandler(void){
	int ch;
	struct dentry* devptr;
	/* complete (or modify) the code s.t. it works */
	while((ch =kbdgetc(devptr)) >= 0) 
	{
		vgaputc(devptr,ch);
	}

}
