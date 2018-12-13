#include <errno.h>
#include <time.h>

#include "darwin.h"

int __platform_gettimeofday(struct timeval *tv, void *tz);

int
clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	struct timeval tval, tz;
	size_t len = sizeof(struct timeval);

	switch (clk_id) {
		case CLOCK_REALTIME:
			if(__platform_gettimeofday(&tval, &tz) < 0)
				break;
			//return -1;
			break;
		case CLOCK_MONOTONIC:
			/* FIXME: should use mach_absolute_time */
			if(__platform_gettimeofday(&tval, &tz) < 0)
				break;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	tp->tv_sec = tval.tv_sec;
	tp->tv_nsec = tval.tv_usec * 1000;

	return 0;
}
