.text
.global start
/* copied from https://opensource.apple.com/source/Csu/Csu-75/start.s.auto.html */
start:	pushq	$0		    # push a zero for debugger end of frames marker
	movq	%rsp,%rbp	    # pointer to base of kernel frame
	andq    $-16,%rsp	    # force SSE alignment
	movq	8(%rbp),%rdi	    # put argc in %rdi
	leaq	16(%rbp),%rsi	    # addr of arg[0], argv, into %rsi
	movl	%edi,%edx	    # copy argc into %rdx
	addl	$1,%edx	    # argc + 1 for zero word
	sall	$3,%edx	    # * sizeof(char *)
	addq	%rsi,%rdx	    # addr of env[0], envp, into %rdx
	movq	%rdx,%rcx
	add	$8,%rcx	    # once found, next pointer is "apple" parameter now in %rcx
	call	__start1
	movl	%eax,%edi	    # pass result from main() to exit() 
	call	__exit		    # need to use call to keep stack aligned
	hlt
