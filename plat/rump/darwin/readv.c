#include <sys/uio.h>
#include "syscall.h"

ssize_t readv(int fd, const struct iovec *iov, int count)
{
	return syscall_3(SYS_readv, fd, iov, count);
}

