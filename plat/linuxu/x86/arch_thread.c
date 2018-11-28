/* SPDX-License-Identifier: MIT */
/*
 ****************************************************************************
 * (C) 2005 - Grzegorz Milos - Intel Research Cambridge
 ****************************************************************************
 *
 *        File: sched.c
 *      Author: Grzegorz Milos
 *     Changes: Robert Kaiser
 *
 *        Date: Aug 2005
 *
 * Environment: Xen Minimal OS
 * Description: simple scheduler for Mini-Os
 *              Ported from Mini-OS
 *
 * The scheduler is non-preemptive (cooperative), and schedules according
 * to Round Robin algorithm.
 *
 ****************************************************************************
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

#include <ucontext.h>
#include <stdlib.h>
#include <uk/arch/thread.h>
#include <uk/plat/config.h>
#include <uk/assert.h>

/* Architecture specific setup of thread creation */
void arch_thread_init(struct ukplat_thread_ctx *ctx, void *stack,
		      void (*function)(void *), void *data)
{
  UK_ASSERT(ctx != NULL);
  UK_ASSERT(stack != NULL);

  makecontext(&ctx->context, function, 1, data);
}

void arch_run_idle_thread(struct ukplat_thread_ctx *ctx)
{
  setcontext(&ctx->context);
}
