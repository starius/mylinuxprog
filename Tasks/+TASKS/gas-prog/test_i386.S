	.file	"test.c"
	.globl	msg
	.section	.rodata
.LC0:
	.string	"test\n"
	.data
	.align 4
	.type	msg, @object
	.size	msg, 4
msg:
	.long	.LC0
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	andl	$-16, %esp
	subl	$16, %esp
	.cfi_offset 3, -12
	movl	msg, %eax
	movl	%eax, (%esp)
	call	strlen
	movl	%eax, %ebx
	movl	%ebx, %edx
	movl	msg, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$1, (%esp)
	call	write
	movl	$7, (%esp)
	call	exit
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.3 20140624 (Red Hat 4.8.3-1)"
	.section	.note.GNU-stack,"",@progbits
