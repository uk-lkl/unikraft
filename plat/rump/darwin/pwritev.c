#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

ssize_t __platform_pwritev(int fd, const struct iovec *iov, int count, off_t offset)
{
	off_t ret = lseek(fd, offset, SEEK_SET);
	if (ret < 0)
		return -1;

	return __platform_writev(fd, iov, count);
}

