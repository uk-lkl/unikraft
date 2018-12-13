#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

#include <sys/types.h>

#define preadv(f, i, n, o) __platform_preadv(f, i, n, o)
#define pwritev(f, i, n, o) __platform_pwritev(f, i, n, o)
#define readv(f, i, n) __platform_readv(f, i, n)
#define writev(f, i, n) __platform_writev(f, i, n)

struct iovec {
	void	*iov_base;
	size_t	 iov_len;
};

ssize_t preadv(int, const struct iovec *, int, off_t);
ssize_t pwritev(int, const struct iovec *, int, off_t);
ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);

#endif
