#include <unistd.h>
#include <errno.h>

off_t lseek(int fd, off_t offset, int whence)
{
	return EINVAL;
}
