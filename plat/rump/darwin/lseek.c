#include <unistd.h>
#include <errno.h>
#include "syscall.h"

off_t lseek(int fd, off_t offset, int whence)
{
	return syscall_3(SYS_lseek, fd, offset, whence);
}
