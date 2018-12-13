#if defined(__x86_64__)
#include "x86_64/syscall.h"
#elif defined(__i386__)
#include "i386/syscall.h"
#elif defined(__ARMEL__) || defined(__ARMEB__)
#include "arm/syscall.h"
#elif defined(__AARCH64EL__) || defined (__AARCH64EB__)
#include "aarch64/syscall.h"
#elif defined(__PPC64__)
#include "ppc64/syscall.h"
#elif defined(__PPC__)
#include "ppc/syscall.h"
#else
#error "Unknown architecture"
#endif

/* from /usr/include/sys/syscall.h */
#define	SYS_exit           1
#define	SYS_read           3
#define	SYS_write          4
#define	SYS_kill           37
#define	SYS_pipe           42
#define	SYS_sigaction      46
#define	SYS_sigprocmask    48
#define	SYS_ioctl          54
#define	SYS_munmap         73
#define	SYS_mprotect       74
#define	SYS_fcntl          92
#define	SYS_select         93
#define	SYS_fsync          95
#define	SYS_gettimeofday   116
#define	SYS_readv          120
#define	SYS_writev         121
#define	SYS_pread          153
#define	SYS_pwrite         154
#define	SYS_fstat          189
#define	SYS_mmap           197
#define	SYS_lseek          199
#define	SYS_sysctl         202
#define	SYS_poll           230
#define	SYS_fstat64        339
