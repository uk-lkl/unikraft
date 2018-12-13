#include <errno.h>
#include <time.h>

#include "darwin.h"

int
clock_getres(clockid_t clk_id, struct timespec *tp)
{

	switch (clk_id) {
		case CLOCK_MONOTONIC:
			tp->tv_nsec = 1000UL;
			tp->tv_sec = 0;
			break;
		case CLOCK_REALTIME:
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}
