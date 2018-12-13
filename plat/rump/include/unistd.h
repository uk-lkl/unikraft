#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stdlib.h>
#include <sys/types.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define fsync(f) __platform_fsync(f)
#define getpagesize __platform_getpagesize
#define pread(a, b, c, d) __platform_pread(a, b, c, d)
#define pwrite(a, b, c, d) __platform_pwrite(a, b, c, d)
#define read(f, b, c) __platform_read(f, b, c)
#define write(f, b, c) __platform_write(f, b, c)
#define lseek(f, o, r) __platform_lseek(f, o, r)
#define pipe(fs) __platform_pipe(fs)

void _exit(int) __attribute__ ((noreturn));
int fsync(int);
int getpagesize(void);
ssize_t pread(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
off_t lseek(int, off_t, int);
int pipe(int fildes[2]);
#endif
