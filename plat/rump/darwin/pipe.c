#include <unistd.h>
#include "syscall.h"

int pipe(int fildes[2])
{
#if 1
	int num = SYS_pipe + 0x2000000;
	int64_t result;
	int64_t flags;

	__asm__ __volatile__ (
		"syscall;\n"
		"movq %%r11, %1;\n"
		: "=a" (result), "=r" (flags)
		: "a" (num), "D" (fildes)
		: "%rcx", "%r11"
		);

	__asm__ __volatile__ (
		"movl	%eax, (%rbx)\n"
		"movl	%edx, 4(%rbx)\n"
		"movl	%eax, %eax\n"
		);

	RETURN_SYSCALL_RESULT(result, flags);
#else
	int ret = syscall_1(SYS_pipe, fildes);
	return ret;
#endif
}
