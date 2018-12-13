#include <poll.h>
#include <select.h>
#include "syscall.h"
#include "init.h"
#include "thread.h"
#include "darwin.h"

/* XXX tidy up */
extern int __platform_npoll;
extern struct pollfd __platform_pollfd[MAXFD];

int sc_select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
	      fd_set *restrict errorfds, struct timeval *restrict timeout);
int sc_poll(struct pollfd *fds, nfds_t nfd, int to)
{
	/* XXX: since character device on darwin can't wait with poll(2),
	 * we need to use select(2) instead.
	 * https://sourceforge.net/p/tuntaposx/code/ci/master/tree/tuntap/README
	 */
	int ret, maxfd = 0;
	unsigned int i;
	struct timeval tval;
	fd_set rfd, wfd, efd;

	if (to >= 0) {
		tval.tv_sec = to / 1000;
		tval.tv_usec = to * 1000 - 1000 * 1000 * tval.tv_sec;
	}

	FD_ZERO(&rfd);
	FD_ZERO(&wfd);
	FD_ZERO(&efd);
	for (i = 0; i < nfd; i++) {
		if (fds[i].events & POLLIN)
			FD_SET(fds[i].fd, &rfd);
		if (fds[i].events & POLLOUT)
			FD_SET(fds[i].fd, &wfd);
		if (fds[i].fd > maxfd)
			maxfd = fds[i].fd;
	}

	ret = sc_select(maxfd + 1, &rfd, &wfd, &efd, to == -1 ? NULL : &tval);

	if (ret > 0) {
		for (i = 0; i < nfd; i++) {
			fds[i].revents = 0;
			if (FD_ISSET(fds[i].fd, &rfd))
				fds[i].revents |= POLLIN;
			if (FD_ISSET(fds[i].fd, &wfd))
				fds[i].revents |= POLLOUT;
		}
	}

	return ret;
}


int poll(struct pollfd *fds, nfds_t n, int timeout)
{
	int i, j, ret = 0;
	int64_t sec;
	long nsec;

	if (timeout == 0)
		return sc_poll(fds, n, timeout);

	if (timeout > 0) {
		sec = timeout / 1000;
		nsec = (timeout % 1000) * (1000*1000UL);
	} else {
		sec = 10;
		nsec = 0;
	}
	clock_sleep(CLOCK_REALTIME, sec, nsec);

	for (i = 0; i < __platform_npoll; i++) {
		if (__platform_pollfd[i].revents) {
			for (j = 0; j < (int)n; j++) {
				if (__platform_pollfd[i].fd == fds[j].fd) {
					fds[j].revents =
						__platform_pollfd[i].revents;
					ret++;
				}
			}
		}
	}

	return ret;
}
