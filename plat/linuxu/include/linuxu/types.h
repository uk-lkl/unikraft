#ifndef __LINUXU_TYPES_H__
#define __LINUXU_TYPES_H__

typedef struct {
  union {
    int __i[14];
    volatile int __vi[14];
    unsigned long __s[7];
  } __u;
} pthread_attr_t;

typedef void * timer_t;

typedef int clockid_t;

typedef long clock_t;

typedef int pid_t;

typedef unsigned uid_t;

#endif /* __LINUXU_TYPES_H__ */
