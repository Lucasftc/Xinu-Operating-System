.code16

LOAD_START_SECTOR= 0x1
LOADER_BASE_ADDRESS =0x900
NUMBER_OF_SECTOR=0x1

.global _start 
_start:# prepare for reading the 1st sector
    movw $0x1f2,%dx
    movb $NUMBER_OF_SECTOR, %al
    outb %al,%dx
    # set the LBA as 01 
    movw $0x1f3,%dx
    movb $LOAD_START_SECTOR, %al
    outb %al,%dx
    movw $0x1f4,%dx
    movb $0x0,%al
    outb %al,%dx
    movw $0x1f5,%dx
    outb %al,%dx

    #set device port and status port
    movw $0x1f6, %dx
    movb $0xe0, %al
    outb %al,%dx
    movw $0x1f7, %dx
    movb $0x20,%al
    movb $0x20,%al
    movb $0x20,%al
    movb $0x20,%al
    outb %al,%dx

.not_ready0:# wait until the read is finished
    nop
    mov $0x1f1, %dx
    inb %dx, %al
    nop
    nop
    nop
    nop
    mov $0x1f7, %dx
    inb %dx,%al
    andb $0x88, %al
    cmpb $0x08, %al# if the 3-bit is 0, it's busy reading the disk
    jne .not_ready0

.go_read1:
    movl $0x0,%ecx
    movl $0x0,%ebx
    movw $0x1f0, %dx
    inw %dx, %ax
    movw %ax,%bx
    inw %dx,%ax
    movw %ax,%cx
    inw %dx, %ax
    movw %ax, %di 
    inw %dx, %ax

    # read the remain of 1st sector
    mov %cx, %si
    mov $252, %cx
.qwq:
    inw %dx, %ax
    loop .qwq
    mov %si, %cx
    mov %di, %ax #

    #calculate the number of sectors to read
    shrl $0x9,%eax
    movl %eax,%esi
    shll $0x9, %esi
    cmpw %si,%di
    je  .prepare
    addl $0x1, %eax
    

.prepare:
    movw $0x1f2,%dx
    outw %ax,%dx
    #prepare for the LBA
    movw $0x1f3,%dx
    movb %bl,%al
    outb %al, %dx
    movw $0x1f4,%dx
    movb %bh,%al
    outb %al, %dx
    movw $0x1f5,%dx
    movb %cl,%al
    outb %al, %dx
    # prepare for the device port and status port
    movw $0x1f6,%dx
    movb $0xe0, %al
    addb %ch,%al
    outb %al,%dx
    movw $0x1f7, %dx
    movb $0x20,%al
    outb %al,%dx

.not_ready2:#wait until the read is finished
    nop
    inb %dx,%al
    andb $0x88,%al
    cmpb $0x08,%al
    jne .not_ready2
    movl %edi, %eax
    shrl $0x1,%eax
    movl %eax, %ecx
    movl $LOADER_BASE_ADDRESS, %ebx
    movw $0x1f0, %dx

.go_read2:
    inw %dx,%ax
    movw %ax,(%ebx)
    addw $0x2, %bx
    loop .go_read2#use loop to read 2 bytes a time

    movw $LOADER_BASE_ADDRESS, %bx
    jmp *%bx


.org 510
.word 0xAA55
