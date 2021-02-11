all: info.img test.img mbr.img
	dd if=info.img of=mbr.img bs=512 count=1 conv=notrunc seek=1
	dd if=test.img of=mbr.img bs=512 count=1 conv=notrunc seek=2

info.img:info.o
	ld -Ttext=0x400 --oformat=binary -o info.img info.o

info.o:info.s
	as -s -o info.o info.s

test.img :test.o
	ld -Ttext=0x900 --oformat=binary -o test.img test.o

test.o:test.s
	as -s -o test.o test.s

mbr.img:boot.o
	ld -Ttext=0x7c00 --oformat=binary -o mbr.img boot.o

boot.o: boot.s
	as -s -o boot.o boot.s

clean:
	rm mbr.img info.img test.img
	rm *.o 
	
