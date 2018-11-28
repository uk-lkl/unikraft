/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Simon Kuenzer <simon.kuenzer@neclab.eu>
 *
 *
 * Copyright (c) 2017, NEC Europe Ltd., NEC Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */

#include <uk/arch/types.h>
#include <uk/arch/limits.h>
#include <uk/config.h>

.section .text

.global _liblinuxuplat_start
_liblinuxuplat_start:
	xorl %ebp, %ebp		# mark the outmost frame (clear the frame pointer)

	popq %rdi		# argc as first argument
	movq %rsp, %rsi		# move argv to rsi, the second parameter in x86_64 abi

	# ignore environ for now

	andq $~15, %rsp		# align stack to 16-byte boundary

	# Run _liblinuxuplat_entry(argc, argv)
	callq _liblinuxuplat_entry

	# Protection
_liblinuxuplat_start_err:
	jmp _liblinuxuplat_start_err

#if HAVE_SCHED
.global thread_starter
thread_starter:
	popq %rdi
	popq %rbx
	pushq $0
	xorq %rbp, %rbp
	call *%rbx
	call exit_thread

.global __arch_switch_threads
__arch_switch_threads:
	pushq %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	movq %rsp, (%rdi) /* save ESP */
	movq (%rsi), %rsp /* restore ESP */
	movq $1f, 8(%rdi) /* sace EIP */
	pushq 8(%rsi)     /* restore EIP */
	ret
1:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
	ret
#endif
