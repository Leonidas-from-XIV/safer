#include "multiboot.h"
#include "safer.h"

static unsigned char *videoram = (unsigned char *)0xb8000;
static char row = 0;
static char line = 0;
static int newline = 0;

typedef unsigned long long int uint64_t;

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

int has_vmx(void) {
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

uint64_t read_msr(int msr) {
    uint64_t result;
    unsigned int high, low;
    asm volatile ( "rdmsr;" : "=d"(high), "=a"(low) : "c"(msr) );
    result = ((uint64_t)high<<32) + low; 
    return result;
}

char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
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
   if (has_vmx()) {
       write_string("VMX detected\n");
   }
   else {
       write_string("No VMX detected\n");
       write_string("Halting");
       while (1) {}
   }

   // we got here, so we are VMX-capable
   uint64_t basic = read_msr(IA32_VMX_BASIC);
   char buffer[80];
   itoa(basic, buffer, 10);
   write_string(buffer);
}
