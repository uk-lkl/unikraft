#include "syscall.h"

int __platform_sysctl(const int *a, unsigned int b, void *c, size_t *d, const void *e, size_t f)
{
	return syscall_6(SYS_sysctl, a, b, c, d, e, f);
}
