#include "syscall.h"

int __platform_fsync(int fd)
{
	return syscall_1(SYS_fsync, fd);
}
