#include <sys/stat.h>
#include <poll.h>
#include <errno.h>

#include "darwin.h"
#include "init.h"
#include "syscall.h"


int __ioctl(int, unsigned long, ...);
int __fstat(int fd, struct darwin_stat *st)
{
	return syscall_2(SYS_fstat64, fd, st);
}

int __platform_npoll = 0;
struct pollfd __platform_pollfd[MAXFD];

int
fstat(int fd, struct stat *st)
{
	int ret, dummy = 1;
	struct darwin_stat dst;

	ret = __fstat(fd, &dst);
	if (ret == -1) {
		errno = EBADF;
		return -1;
	}

	st->st_size = dst.st_size;

	switch (dst.st_mode & DARWIN_S_IFMT) {
	case DARWIN_S_IFBLK:
		__ioctl(fd, DIOCGMEDIASIZE, &dst.st_size);
		break;
	case DARWIN_S_IFCHR:
		/* XXX this only works fine with tuntaposx */
		/* XXX: this degrades to detect fd0,1,2 as sock... */
		ret = __ioctl(fd, FIOASYNC, &dummy);
		if ((ret == -1) && (errno == ENOTTY)) {
			/* say we are a "socket" ie network device */
			dst.st_mode = DARWIN_S_IFSOCK;
			/* add to poll */
			__platform_pollfd[__platform_npoll].fd = fd;
			__platform_pollfd[__platform_npoll].events = POLLIN | POLLPRI;
			__platform_npoll++;
		}
		break;
	case DARWIN_S_IFREG:
		if (fd == 4 || __franken_fd[fd].mounted == 1) {
			dst.st_mode &= ~DARWIN_S_IFMT;
			dst.st_mode |= DARWIN_S_IFBLK;
		}
	}

	st->st_mode = (DARWIN_S_ISDIR (dst.st_mode) ? S_IFDIR  : 0) |
		      (DARWIN_S_ISCHR (dst.st_mode) ? S_IFCHR  : 0) |
		      (DARWIN_S_ISBLK (dst.st_mode) ? S_IFBLK  : 0) |
		      (DARWIN_S_ISREG (dst.st_mode) ? S_IFREG  : 0) |
		      (DARWIN_S_ISFIFO(dst.st_mode) ? S_IFIFO  : 0) |
		      (DARWIN_S_ISLNK (dst.st_mode) ? S_IFLNK  : 0) |
		      (DARWIN_S_ISSOCK(dst.st_mode) ? S_IFSOCK : 0);

	return 0;
}
