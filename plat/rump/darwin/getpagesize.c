#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "darwin.h"

int __platform_sysctl(const int *, unsigned int, void *, size_t *, const void *, size_t);

#ifdef HUGEPAGESIZE
int
getpagesize(void)
{

	return HUGEPAGESIZE;
}
#else
int
getpagesize(void)
{
	const int ctl[2] = {CTL_HW, HW_PAGESIZE};
	static int pgsize = 4096;
	size_t lenp = sizeof(int);
	int res;

	if (pgsize > 0)
		return pgsize;

	res = __platform_sysctl(ctl, 2, &pgsize, &lenp, NULL, 0);
	if (res == -1)
		kill(0, SIGABRT);

	return pgsize;
}
#endif
