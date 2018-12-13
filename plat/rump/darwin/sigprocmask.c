#include "syscall.h"

typedef uint32_t	sigset_t; /* __darwin_sigset_t */

int __platform_sigprocmask(int how, const sigset_t *restrict set,
			   sigset_t *restrict oset)
{
	return syscall_3(SYS_sigprocmask, how, set, oset);
}
