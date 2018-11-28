#ifndef __LINUXU_TIME_H__
#define __LINUXU_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>

#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3
#define CLOCK_MONOTONIC_RAW      4
#define CLOCK_REALTIME_COARSE    5
#define CLOCK_MONOTONIC_COARSE   6
#define CLOCK_BOOTTIME           7
#define CLOCK_REALTIME_ALARM     8
#define CLOCK_BOOTTIME_ALARM     9
#define CLOCK_SGI_CYCLE         10
#define CLOCK_TAI               11

struct itimerspec
{
  struct timespec it_interval;
  struct timespec it_value;
};

#ifdef __cplusplus
}
#endif

#endif /* __LINUXU_TIME_H__ */
