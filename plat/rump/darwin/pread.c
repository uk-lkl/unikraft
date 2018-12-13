#include <sys/uio.h>
#include "syscall.h"

ssize_t __platform_pread(int fd, void *buf, size_t count, off_t offset)
{
	return syscall_4(SYS_pread, fd, buf, count, offset);
}

