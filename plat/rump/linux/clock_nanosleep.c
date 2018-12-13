#include <time.h>
#include <errno.h>

#include "syscall.h"

#include "linux.h"
#include "init.h"
#include "thread.h"

/* XXX tidy up */
extern int __platform_npoll;
extern struct pollfd __platform_pollfd[MAXFD];

#define NSEC_PER_SEC	1000000000L
#define MIN_SLEEP_INTERVAL_NSEC (60*1000)

static long long timespec_diff_ns(struct timespec start, struct timespec end)
{
	struct timespec temp;

	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = NSEC_PER_SEC + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return (temp.tv_sec * NSEC_PER_SEC) + temp.tv_nsec;
}


static int sleep_fast_path(struct timespec *ltp)
{
	struct timespec start, now;
	int ret;
	int i = 0;

/*
 * XXX: the fast path for sleeping for a short period of time
 *
 * I don't know if this is a correct way to sleep but given the clock_gettime
 * uses vdso and less expensive than clock_nanosleep, which requires 60 usec
 * (in my environment) at minimum, and the busy loop should end in a short
 * duration, this could give a cheap sleep for the particular environment.
 */
	ret = clock_gettime(CLOCK_MONOTONIC, &start);
	if (ret != 0) {
		errno = EINVAL;
		return -1;
	}

	while (1) {
		ret = clock_gettime(CLOCK_MONOTONIC, &now);
		if (ret != 0) {
			errno = EINVAL;
			return -1;
		}

		if (timespec_diff_ns(start, now) >
		    ltp->tv_sec * NSEC_PER_SEC + ltp->tv_nsec)
			break;
	}
	return 0;
}

int clock_nanosleep(clockid_t clk_id, int flags, const struct timespec *request, struct timespec *remain)
{
	clockid_t lid;
	struct linux_timespec ltp;
	int ret, i;
	struct thread *thread;

	ltp.tv_sec = request->tv_sec;
	ltp.tv_nsec = request->tv_nsec;

	/* we could support these but not needed */
	if (flags != 0 || remain != NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (clk_id) {
		case CLOCK_REALTIME:
			lid = LINUX_CLOCK_REALTIME;
			break;
		case CLOCK_MONOTONIC:
			lid = LINUX_CLOCK_MONOTONIC;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	if (__platform_npoll == 0) {
		/* Since syscall(clock_nanosleep) costs around 60 usec with
		 * my machine (Linux 4.8, core i7-2.5GHz), return earlier if the
		 * sleep interval is less than 60 usec
		 */
		if ((ltp.tv_sec * NSEC_PER_SEC + ltp.tv_nsec) <
		    MIN_SLEEP_INTERVAL_NSEC) {
			return sleep_fast_path(&ltp);
		}

		ret = syscall(SYS_clock_nanosleep, lid, 0, &ltp, NULL);
		if (ret != 0) {
			errno = EINVAL;
			return -1;
		}
		return 0;
	}

	/* use poll instead as we might have network events */

	ret = syscall(SYS_ppoll, __platform_pollfd, __platform_npoll, &ltp, NULL);
	if (ret == -1) {
		errno = EINVAL;
		return -1;
	}

	if (ret == 0)
		return 0;

	for (i = 0; i < __platform_npoll; i++) {
		if (__platform_pollfd[i].revents) {
			thread = __franken_fd[__platform_pollfd[i].fd].wake;
			if (thread)
				wake(thread);
		}
	}

	return 0;
}
