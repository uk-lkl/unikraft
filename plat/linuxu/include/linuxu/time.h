#ifndef __LINUXU_TIME_H__
#define __LINUXU_TIME_H__

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct itimerspec
{
  struct timespec it_interval;
  struct timespec it_value;
};

#ifdef __cplusplus
}
#endif

#endif /* __LINUXU_TIME_H__ */
