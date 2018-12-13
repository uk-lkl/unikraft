#include <sys/uio.h>
#include "syscall.h"

ssize_t __platform_pwrite(int fd, const void *buf, size_t count, off_t offset)
{
	return syscall_4(SYS_pwrite, fd, buf, count, offset);
}

