#include "xinu.h"
#include "kbd.h"

uint16 get_cursor(){
    uint16 pos;
    outb(0x3d4,14);
    pos=inb(0x3d5)<<8;
    outb(0x3d4,15);
    pos|=inb(0x3d5);
}
void set_cursor(uint16 pos){
    outb(0x3d4,14);
    outb(0x3d5,pos>>8);
    outb(0x3d4,15);
    outb(0x3d5,pos);
}
uint32 line_roundup(uint16 pos){
    uint16 newpos;
    newpos=((pos+80)/80)*80;
    return newpos;
}
devcall vgaputc(struct dentry *devptr, char ch){
    static uint32 pos=0;
    uint32 base=0xb8000;
    static uint16 *ptr=0xb800;
    char blank=' ';
    int i,j,k;
    if(ch==-1 || ch==0)
        return ;
    if(ch=='\b'){
        if(pos%80==0){
            if(pos==0)
                return OK;
            pos=(pos/80)*80-1;
            set_cursor(pos);
            return OK;
        }
        pos--;
        ptr=(uint16*)(base+pos*2);
        *ptr=(blank & 0xff)|0x0700;
        set_cursor(pos);
    }
    else if(ch=='\r' || ch=='\n'){
        pos=get_cursor();
        pos=line_roundup(pos);
        if(pos==24*80){
            int i;
            for(i=0;i<23*160;i+=2){
                uint16* oldline;
                uint16* newline;
                oldline=(uint16*)(base+i);
                newline=(uint16*)(base+i+160);
                *oldline=*newline;
            }
            for(i=23*160;i<24*160;i+=2){
                ptr=(uint16*)(base+i);
                *ptr=(blank & 0xff)|0x0700;
            }
            set_cursor(23*80);
            pos=23*80;
        }
        else{
            set_cursor(pos);
        }
    }
    else {
        ptr=(uint16*)(base+pos*2);
        *ptr=(ch& 0xff)|0x0700;
        pos+=1;
        set_cursor(pos);

    }
}