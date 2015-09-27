	.file	"test.c"
	.globl	msg
	.section	.rodata
.LC0:
	.string	"test\n"
	.data
	.align 8
	.type	msg, @object
	.size	msg, 8
msg:
	.quad	.LC0
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movq	msg(%rip), %rax
	movq	%rax, %rdi
	call	strlen
	movl	%eax, %ebx
	movslq	%ebx, %rdx
	movq	msg(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write
	movl	$7, %edi
	call	exit
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.3 20140624 (Red Hat 4.8.3-1)"
	.section	.note.GNU-stack,"",@progbits
