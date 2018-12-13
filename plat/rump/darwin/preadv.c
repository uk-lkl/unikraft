#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "syscall.h"

ssize_t __platform_preadv(int fd, const struct iovec *iov, int iovcnt, off_t off)
{
	off_t ret = lseek(fd, off, SEEK_SET);
	if (ret < 0)
		return -1;

	return __platform_readv(fd, iov, iovcnt);
}

