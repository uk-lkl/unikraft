#include "syscall.h"

int __platform_kill(pid_t pid, int sig)
{
	return syscall_2(SYS_kill, pid, sig);
}
