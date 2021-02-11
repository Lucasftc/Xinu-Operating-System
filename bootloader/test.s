.code16
.global _start
_start:
    movw %cs,%ax
    movw %ax,%ds
    movw %ax,%es
    movw $0x7e00, %sp
    movw $0xb800,%ax
    movw %ax,%gs

    movw $0x600, %ax
    movw $0x700,%bx
    movw $0,%cx
    movw $0x184f, %dx
    int $0x10

    movb $'1',%gs:00
    movb $0xA4, %gs:01

    movb $'M',%gs:02
    movb $0xA4,%gs:03

    movb $'B',%gs:04
    movb $0xA4, %gs:05

    movb $'R',%gs:06
    movb $0xA4, %gs:07

.hang:
    jmp .hang

.org 510
.word 0xAA55