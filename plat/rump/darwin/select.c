#include <select.h>
#include "syscall.h"

int sc_select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
	      fd_set *restrict errorfds, struct timeval *restrict timeout)
{
	return syscall_5(SYS_select, nfds, readfds, writefds,
			 errorfds, timeout);
}
