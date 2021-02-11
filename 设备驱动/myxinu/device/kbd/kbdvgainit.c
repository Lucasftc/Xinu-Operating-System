#include "xinu.h"
#include "kbd.h"

extern void set_cursor(uint16);
devcall kbdvgainit(void){
    uint16* ptr;
    char blank=' ';
    uint32 base=0xb8000;
    int i=0;
    for(i=0;i<24*160;i+=2){
        ptr=(uint16*)(base+i);
        *ptr=(blank&0xff)|0x0700;
    }
    set_cursor(0);
    set_evec(33, kbddisp);
}