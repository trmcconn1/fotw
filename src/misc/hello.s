	.file	"hello.c"
	.version	"01.01"
gcc2_compiled.:
		.section	.rodata
.LC0:
	.string	"hello world\n"
.text
	.align 4
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	$0, %eax
	leave
	ret
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) 2.96 20000731 (Red Hat Linux 7.0)"
