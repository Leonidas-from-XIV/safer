CFLAGS= -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -m32 -O0 -g
LDFLAGS= -T linker.ld -melf_i386
ASFLAGS= --32

OBJS= loader.o kernel.o

all: kernel

kernel: $(OBJS)
	ld -o $@ $(LDFLAGS) $(OBJS)

qemu: all
	qemu-kvm -kernel kernel -net none -cpu host

clean:
	git clean -fx

.PHONY: qemu clean
