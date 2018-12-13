#include <signal.h>
#include "syscall.h"

int __platform_sigaction(int sig, const struct sigaction *restrict act,
			 struct sigaction *restrict oact)
{
	return syscall_3(SYS_sigaction, sig, act, oact);
}
