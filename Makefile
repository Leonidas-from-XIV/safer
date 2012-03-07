CFLAGS=-Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -m32
LDFLAGS=-T linker.ld -melf_i386
ASFLAGS=--32

all: kernel

kernel: loader.o kernel.o
	ld -o $@ $(LDFLAGS) loader.o kernel.o

loader.o: loader.s

kernel.o: kernel.c

qemu: all
	qemu-kvm -kernel kernel -net none -cpu host

clean:
	git clean -fx

.PHONY: qemu clean
