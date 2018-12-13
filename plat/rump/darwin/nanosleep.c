#include <sys/types.h>
#include <time.h>
#include <select.h>
#include "darwin.h"

int sc_select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
	      fd_set *restrict errorfds, struct timeval *restrict timeout);
int __platform_nanosleep(const struct timespec *req, struct timespec *rem)
{
	struct timeval to;

	to.tv_sec = req->tv_sec;
	to.tv_usec = req->tv_nsec / 1000;

	return sc_select(0, NULL, NULL, NULL, &to);
}

