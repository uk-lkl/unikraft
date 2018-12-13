#ifndef _POLL_H_
#define _POLL_H_

#include <sys/types.h>

typedef unsigned int nfds_t;
struct pollfd {
	int	fd;
	short	events;
	short	revents;
};

#define poll(f, n, t) __platform_poll(f, n, t)
int poll(struct pollfd *f, nfds_t n, int t);

#define POLLIN     0x001
#define POLLPRI    0x002
#define POLLOUT    0x004
#define POLLERR    0x008
#define POLLHUP    0x010
#define POLLNVAL   0x020

#endif /* _POLL_H_ */
