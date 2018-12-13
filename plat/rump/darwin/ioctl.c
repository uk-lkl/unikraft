#include <sys/types.h>
#include <stdarg.h>
#include "syscall.h"

int __ioctl(int fd, unsigned long cmd, ...)
{
	unsigned long arg;
	va_list ap;

	va_start(ap, cmd);
	arg = va_arg(ap, unsigned long);
	va_end(ap);

	return syscall_3(SYS_ioctl, fd, cmd, arg);
}
