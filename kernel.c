#include "multiboot.h"

static unsigned char *videoram = (unsigned char *)0xb8000;
static char row = 0;
static char line = 0;
static int newline = 0;

void write_string(const char *string)
{
    int position;
    int i;
    while(*string != 0)
    {
        if (row >= 80 || *string == '\n') {
            line++;
            row = 0;
            newline = 1;
        }
        if (line >= 24 && newline) {
            newline = 0;
            // scroll
            for (i = 80*2; i < 25*80*2; i+=2) {
                videoram[i-2*80] = videoram[i];
                videoram[i-2*80+1] = videoram[i+1];
            }
            line = 24;
            // blank lowest line
            for (i = 0; i < 80; i+=2) {
                videoram[24*80*2+i] = ' ';
            }
        }

        if (*string == '\n') {
            string++;
            continue;
        }

        position = line*80*2+row*2;

        videoram[position] = *string++;
        videoram[position+1] = 0x07;
        row++;
    }
}

int hasvmx(void) {
    int flags;
    /* put 1 into EAX and run CPUID instruction and read ECX into flags */
    asm volatile(
        "movl $1, %%eax;"
        "cpuid"
        : "=c" (flags)
    );
    /* VMX is bit 5 of ECX */
    return flags & 0x20;
}

void kmain(void* mbd, unsigned int magic)
{
   if (magic != 0x2BADB002)
   {
      /* Something went not according to specs. Print an error */
      /* message and halt, but do *not* rely on the multiboot */
      /* data structure. */
   }

   /* You could either use multiboot.h */
   /* (http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#multiboot_002eh) */
   /* or do your offsets yourself. The following is merely an example. */
   //char * boot_loader_name =(char*) ((long*)mbd)[16];
   multiboot_info_t* mbi = mbd;
   write_string("Running hypervisor set up...\n");
   char message[] = "Hello World! ";
   write_string(message);
   write_string("\nBootloader: \"");
   write_string((char*)mbi->boot_loader_name);
   write_string("\"\n");

   //write_string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

   while (0) {
       write_string(message);
   }

   write_string("VMX detection: ");
   if (hasvmx()) {
       write_string("VMX detected");
   }
   else {
       write_string("No VMX detected");
   }
   write_string("\n");
}
