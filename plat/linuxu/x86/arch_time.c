/* SPDX-License-Identifier: MIT */
/*
 ****************************************************************************
 * (C) 2003 - Rolf Neugebauer - Intel Research Cambridge
 * (C) 2002-2003 - Keir Fraser - University of Cambridge
 * (C) 2005 - Grzegorz Milos - Intel Research Cambridge
 * (C) 2006 - Robert Kaiser - FH Wiesbaden
 ****************************************************************************
 *
 *        File: time.c
 *      Author: Rolf Neugebauer and Keir Fraser
 *     Changes: Grzegorz Milos
 *
 * Description: Simple time and timer functions
 *              Ported from Mini-OS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdint.h>
#include <sys/time.h>
#include <linuxu/syscall.h>
#include <linuxu/signal.h>
#include <linuxu/time.h>
#include <linuxu/types.h>
#include <uk/plat/time.h>
#include <uk/assert.h>

/************************************************************************
 * Time functions
 *************************************************************************/
static struct sigaction sigact;
static struct sigevent sigevp;
static struct itimerspec ispec;
static timer_t timerid = 0;
static volatile uint64_t ticks = 0;

/* XXX: from musl/src/signal/sigemptyset.c */
static int sigemptyset(sigset_t *set)
{
  set->__bits[0] = 0;
  if (sizeof(long)==4 || _NSIG > 65) set->__bits[1] = 0;
  if (sizeof(long)==4 && _NSIG > 65) {
    set->__bits[2] = 0;
    set->__bits[3] = 0;
  }
  return 0;
}

static void timer_handler(__attribute__ ((unused)) int signum,
                          __attribute__ ((unused)) siginfo_t *info,
                          __attribute__ ((unused)) void *ctx)
{
  ticks += 10;
}

/* monotonic_clock(): returns # of nanoseconds passed since time_init()
 *		Note: This function is required to return accurate
 *		time even in the absence of multiple timer ticks.
 */
__nsec ukplat_monotonic_clock(void)
{
  return ticks;
}

void ukplat_time_init(void)
{
  int ret;
  sigact.sa_sigaction = timer_handler;
  sigact.sa_flags = SA_SIGINFO | SA_RESTART;
  sigemptyset(&sigact.sa_mask);
  ret = sys_sigaction(SIGRTMIN + 1, &sigact, NULL);
  if (ret < 0) {
    uk_printk("Error sigaction: %d\n", ret);
    return;
  }

  sigevp.sigev_notify = SIGEV_SIGNAL;
  sigevp.sigev_signo = SIGRTMIN + 1;
  ret = sys_timer_create(CLOCK_REALTIME, &sigevp, &timerid);
  if (ret < 0) {
    uk_printk("Error timer_create: %d\n", ret);
    return;
  }

  ispec.it_interval.tv_sec = 0;
  ispec.it_interval.tv_nsec = 10000000;
  ispec.it_value.tv_sec = 0;
  ispec.it_value.tv_nsec = 0;
  ret = sys_timer_settime(timerid, 0, &ispec, NULL);
  if (ret < 0) {
    uk_printk("Error timer_create: %d\n", ret);
    return;
  }
}

void ukplat_time_fini(void)
{
  int ret = sys_timer_delete(timerid);
  if (ret < 0) {
    uk_printk("Error time_delete: %d\n", ret);
  }
}

#ifdef CONFIG_MIGRATION /* TODO wip */
void suspend_time(void)
{
}

void resume_time(void)
{
}
#endif
