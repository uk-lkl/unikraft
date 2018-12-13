#include <unistd.h>

void _exit(int status)
{
	if (status == 0) {
		__asm__ __volatile__("mov r0, #0x18;\n"
				     "ldr r1, =0x20026\n"
				     "svc 0x00123456;\n"
				     ::: "r0", "r1");
	}

	__asm__ __volatile__("mov r0, #0x18;\n"
			     "mov r1, %0\n"
			     "svc 0x00123456;\n"
			     :: "r"(status)
			     : "r0", "r1");
}
