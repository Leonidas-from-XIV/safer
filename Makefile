kernel: loader.o kernel.o
	ld -o kernel -T linker.ld loader.o kernel.o -melf_i386 

loader.o:
	as -o loader.o loader.s --32

kernel.o:
	gcc -o kernel.o -c kernel.c -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -m32

qemu: kernel
	qemu-kvm -kernel kernel -net none -cpu host
