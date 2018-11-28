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
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <lk/kernel/thread.h>
#include <lk/kernel/event.h>
#include <lk/kernel/timer.h>
#include <uk/plat/time.h>
#include <common/events.h>
#include <uk/assert.h>

/************************************************************************
 * Time functions
 *************************************************************************/
static struct sigaction sigact;
static struct sigevent sigevp;
static struct itimerspec ispec;
static timer_t timerid = 0;
static volatile uint64_t ticks = 0;

static void timer_handler(int signum, siginfo_t *info, void *ctx)
{
  ticks += 10;
  if (thread_timer_tick() == INT_RESCHEDULE)
    thread_preempt();
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
  sigact.sa_sigaction = timer_handler;
  sigact.sa_flags = SA_SIGINFO | SA_RESTART;
  sigemptyset(&sigact.sa_mask);
  if (sigaction(SIGRTMIN + 1, &sigact, NULL) < 0) {
    perror("sigaction error");
    exit(1);
  }

  sigevp.sigev_notify = SIGEV_SIGNAL;
  sigevp.sigev_signo = SIGRTMIN + 1;
  if (timer_create(CLOCK_REALTIME, &sigevp, &timerid) < 0) {
    perror("timer_create error");
    exit(1);
  }

  ispec.it_interval.tv_sec = 0;
  ispec.it_interval.tc_nsec = 10000000;
  ispec.it_value.tv_sec = 0;
  ispec.it_value.tv_nsec = 0;
  if (timer_settime(timerid, 0, &ispec, NULL) < 0) {
    perror("timer_settime error");
    exit(1);
  }
}

void ukplat_time_fini(void)
{
  if (timer_delete(timerid) < 0) {
    perror("timer_delete error");
    exit(1);
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
