#include <poll.h>
#include "syscall.h"
#include "init.h"
#include "thread.h"

/* XXX tidy up */
extern int __platform_npoll;
extern struct pollfd __platform_pollfd[MAXFD];
int sc_poll(struct pollfd *, nfds_t, int);

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
