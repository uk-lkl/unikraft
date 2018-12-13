#include "syscall.h"
#include <sys/mman.h>

int munmap(void *mem, size_t size)
{
	return syscall_2(SYS_munmap, mem, size);
}
