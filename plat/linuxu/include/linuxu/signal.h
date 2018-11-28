#ifndef __LINUXU_SIGNAL_H__
#define __LINUXU_SIGNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <linuxu/types.h>

union sigval {
  int sigval_int;
  void *sigval_ptr;
};

typedef struct {
  int si_signo, si_errno, si_code;
  union {
    char __pad[128 - 2*sizeof(int) - sizeof(long)];
    struct {
      union {
        struct {
          pid_t si_pid;
          uid_t si_uid;
        } __piduid;
        struct {
          int si_timerid;
          int si_overrun;
        } __timer;
      } __first;
      union {
        union sigval si_value;
        struct {
          int si_status;
          clock_t si_utime, si_stime;
        } __sigchld;
      } __second;
    } __si_common;
    struct {
      void *si_addr;
      short si_addr_lsb;
      struct {
        void *si_lower;
        void *si_upper;
      } __addr_bnd;
    } __sigfault;
    struct {
      long si_band;
      int si_fd;
    } __sigpoll;
    struct {
      void *si_call_addr;
      int si_syscall;
      unsigned si_arch;
    } __sigsys;
  } __si_fields;
} siginfo_t;

typedef struct __sigset_t {
  unsigned long __bits[128/sizeof(long)];
} sigset_t;

struct sigaction {
  union {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
  } __sa_handler;
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
};
#define sa_handler    __sa_handler.sa_handler
#define sa_sigaction  __sa_handler.sa_sigaction

struct sigevent {
  union sigval sigev_value;
  int sigev_signo;
  int sigev_notify;
  void (*sigev_notify_function)(union sigval);
  pthread_attr_t *sigev_notify_attributes;
  char __pad[56-3*sizeof(long)];
};

#ifdef __cplusplus
}
#endif

#endif /* __LINUXU_SIGNAL_H__ */
