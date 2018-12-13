#ifndef _SELECT_H_
#define _SELECT_H_

#include <sys/types.h>

#define FD_ZERO(s) do {						\
		int __i; unsigned long *__b=(s)->fds_bits;		\
		for(__i=sizeof (fd_set)/sizeof (long); __i; __i--) *__b++=0; \
	} while(0)
#define FD_SET(d, s)   ((s)->fds_bits[(d)/(8*sizeof(long))] |= (1UL<<((d)%(8*sizeof(long)))))
#define FD_CLR(d, s)   ((s)->fds_bits[(d)/(8*sizeof(long))] &= ~(1UL<<((d)%(8*sizeof(long)))))
#define FD_ISSET(d, s) !!((s)->fds_bits[(d)/(8*sizeof(long))] & (1UL<<((d)%(8*sizeof(long)))))

typedef struct fd_set {
	int32_t       fds_bits[1024];
} fd_set;

#endif /* _SELECT_H_ */
