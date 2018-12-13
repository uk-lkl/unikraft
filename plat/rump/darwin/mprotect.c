#include "syscall.h"

int mprotect(void *mem, size_t size, int prot)
{
	return syscall_3(SYS_mprotect, mem, size, prot);
}
