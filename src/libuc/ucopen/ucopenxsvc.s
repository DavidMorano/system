	.build_version macos,  10, 15
	.text
	.const
	.align 4
__ZL9UUID_NULL:
	.space 16
	.text
	.globl __Z8memclearPvi
	.weak_definition __Z8memclearPvi
__Z8memclearPvi:
LFB5552:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	-28(%rbp), %eax
	cltq
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	_memset
	movl	-28(%rbp), %eax
	leave
LCFI2:
	ret
LFE5552:
	.const
__ZL10machendian:
	.space 1
	.text
	.align 1,0x90
	.globl __ZN6ids_coclEP3idsi
	.weak_definition __ZN6ids_coclEP3idsi
__ZN6ids_coclEP3idsi:
LFB5900:
	pushq	%rbp
LCFI3:
	movq	%rsp, %rbp
LCFI4:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %edx
	movl	%edx, 8(%rax)
	nop
	popq	%rbp
LCFI5:
	ret
LFE5900:
	.align 1,0x90
	.globl __ZN6ids_coC1Ev
	.weak_definition __ZN6ids_coC1Ev
__ZN6ids_coC1Ev:
LFB5905:
	pushq	%rbp
LCFI6:
	movq	%rsp, %rbp
LCFI7:
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	movq	-8(%rbp), %rax
	movl	$-1, 8(%rax)
	nop
	popq	%rbp
LCFI8:
	ret
LFE5905:
	.align 1,0x90
	.globl __ZN3idsC1Ev
	.weak_definition __ZN3idsC1Ev
__ZN3idsC1Ev:
LFB5907:
	pushq	%rbp
LCFI9:
	movq	%rsp, %rbp
LCFI10:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	__ZN6ids_coC1Ev
	movq	-8(%rbp), %rax
	addq	$40, %rax
	movq	%rax, %rdi
	call	__ZN6ids_coC1Ev
	movq	-8(%rbp), %rax
	addq	$56, %rax
	movq	%rax, %rdi
	call	__ZN6ids_coC1Ev
	movq	-8(%rbp), %rax
	addq	$72, %rax
	movq	%rax, %rdi
	call	__ZN6ids_coC1Ev
	movq	-8(%rbp), %rax
	addq	$24, %rax
	movq	-8(%rbp), %rcx
	movl	$0, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	__ZN6ids_coclEP3idsi
	movq	-8(%rbp), %rax
	addq	$40, %rax
	movq	-8(%rbp), %rcx
	movl	$1, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	__ZN6ids_coclEP3idsi
	movq	-8(%rbp), %rax
	addq	$56, %rax
	movq	-8(%rbp), %rcx
	movl	$2, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	__ZN6ids_coclEP3idsi
	movq	-8(%rbp), %rax
	addq	$72, %rax
	movq	-8(%rbp), %rcx
	movl	$3, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	__ZN6ids_coclEP3idsi
	nop
	leave
LCFI11:
	ret
LFE5907:
	.align 1,0x90
	.globl __ZN3idsD1Ev
	.weak_definition __ZN3idsD1Ev
__ZN3idsD1Ev:
LFB5910:
	pushq	%rbp
LCFI12:
	movq	%rsp, %rbp
LCFI13:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	__ZN3ids4dtorEv
	nop
	leave
LCFI14:
	ret
LFE5910:
_isOneOf:
LFB5957:
	pushq	%rbp
LCFI15:
	movq	%rsp, %rbp
LCFI16:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	_isoneof
	leave
LCFI17:
	ret
LFE5957:
	.zerofill __DATA,__bss,__ZL3var,8,3
	.cstring
lC0:
	.ascii "so\0"
lC1:
	.ascii "o\0"
lC2:
	.ascii "\0"
	.const_data
	.align 5
__ZL6soexts:
	.quad	lC0
	.quad	lC1
	.quad	lC2
	.quad	0
	.const
	.align 3
__ZL7rsnoacc:
	.long	-2
	.long	-13
	.long	0
	.zerofill __DATA,__bss,__ZZ11uc_openxsvcP8openxsvcE3rsv,4,2
	.zerofill __DATA,__bss,__ZGVZ11uc_openxsvcP8openxsvcE3rsv,8,3
	.text
	.align 1,0x90
	.globl __ZN7subinfoC1Ev
	.weak_definition __ZN7subinfoC1Ev
__ZN7subinfoC1Ev:
LFB5967:
	pushq	%rbp
LCFI18:
	movq	%rsp, %rbp
LCFI19:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	__ZN3idsC1Ev
	nop
	leave
LCFI20:
	ret
LFE5967:
	.align 1,0x90
	.globl __ZN7subinfoD1Ev
	.weak_definition __ZN7subinfoD1Ev
__ZN7subinfoD1Ev:
LFB5970:
	pushq	%rbp
LCFI21:
	movq	%rsp, %rbp
LCFI22:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	__ZN3idsD1Ev
	nop
	leave
LCFI23:
	ret
LFE5970:
	.globl __Z11uc_openxsvcP8openxsvc
__Z11uc_openxsvcP8openxsvc:
LFB5964:
	pushq	%rbp
LCFI24:
	movq	%rsp, %rbp
LCFI25:
	subq	$208, %rsp
	movq	%rdi, -200(%rbp)
	movl	$-14, -4(%rbp)
	movl	$-1, -8(%rbp)
	cmpq	$0, -200(%rbp)
	je	L12
	movq	-200(%rbp), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	L12
	movq	-200(%rbp), %rax
	movq	16(%rax), %rax
	testq	%rax, %rax
	je	L12
	movq	-200(%rbp), %rax
	movq	24(%rax), %rax
	testq	%rax, %rax
	je	L12
	movl	$-22, -4(%rbp)
	movq	-200(%rbp), %rax
	movq	8(%rax), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	L12
	movq	-200(%rbp), %rax
	movq	16(%rax), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	L12
	movq	-200(%rbp), %rax
	movq	24(%rax), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	L12
	movzbl	__ZGVZ11uc_openxsvcP8openxsvcE3rsv(%rip), %eax
	testb	%al, %al
	sete	%al
	testb	%al, %al
	je	L13
	leaq	__ZGVZ11uc_openxsvcP8openxsvcE3rsv(%rip), %rax
	movq	%rax, %rdi
	call	___cxa_guard_acquire
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L13
	leaq	__ZL3var(%rip), %rax
	movq	%rax, %rdi
	call	__ZN12_GLOBAL__N_14varscviEv
	movl	%eax, __ZZ11uc_openxsvcP8openxsvcE3rsv(%rip)
	leaq	__ZGVZ11uc_openxsvcP8openxsvcE3rsv(%rip), %rax
	movq	%rax, %rdi
	call	___cxa_guard_release
L13:
	movl	__ZZ11uc_openxsvcP8openxsvcE3rsv(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L12
	leaq	-192(%rbp), %rax
	movq	%rax, %rdi
	call	__ZN7subinfoC1Ev
	movq	-200(%rbp), %rdx
	leaq	-192(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	__ZL13subinfo_startP7subinfoP8openxsvc
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L14
	leaq	-192(%rbp), %rax
	movq	%rax, %rdi
	call	__ZL14subinfo_searchP7subinfo
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	setg	%al
	testb	%al, %al
	je	L15
	movl	-28(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	L16
L15:
	cmpl	$0, -4(%rbp)
	jne	L16
	movl	$-2, -4(%rbp)
L16:
	leaq	-192(%rbp), %rax
	movq	%rax, %rdi
	call	__ZL14subinfo_finishP7subinfo
	movl	%eax, -12(%rbp)
	cmpl	$0, -4(%rbp)
	js	L17
	movl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
L17:
	cmpl	$0, -4(%rbp)
	jns	L14
	cmpl	$0, -8(%rbp)
	js	L14
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	_u_close
L14:
	leaq	-192(%rbp), %rax
	movq	%rax, %rdi
	call	__ZN7subinfoD1Ev
L12:
	cmpl	$0, -4(%rbp)
	js	L18
	movl	-8(%rbp), %eax
	jmp	L20
L18:
	movl	-4(%rbp), %eax
L20:
	leave
LCFI26:
	ret
LFE5964:
__ZL13subinfo_startP7subinfoP8openxsvc:
LFB5971:
	pushq	%rbp
LCFI27:
	movq	%rsp, %rbp
LCFI28:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	__Z8memclearI7subinfoEiPT_
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L22
	movq	-32(%rbp), %rax
	movq	8(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 96(%rax)
	movq	-32(%rbp), %rax
	movq	16(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 104(%rax)
	movq	-32(%rbp), %rax
	movq	24(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 112(%rax)
	movq	-32(%rbp), %rax
	movq	32(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 136(%rax)
	movq	-32(%rbp), %rax
	movq	40(%rax), %rax
	testq	%rax, %rax
	je	L23
	movq	-32(%rbp), %rax
	movq	40(%rax), %rax
	jmp	L24
L23:
	movq	_environ@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
L24:
	movq	-24(%rbp), %rdx
	movq	%rax, 144(%rdx)
	movq	-32(%rbp), %rax
	movl	48(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 156(%rax)
	movq	-32(%rbp), %rax
	movzwl	56(%rax), %edx
	movq	-24(%rbp), %rax
	movw	%dx, 152(%rax)
	movq	-32(%rbp), %rax
	movl	52(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 160(%rax)
	movq	-24(%rbp), %rax
	movl	$-1, 164(%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	__ZL14subinfo_startsP7subinfo
	movl	%eax, -4(%rbp)
L22:
	movl	-4(%rbp), %eax
	leave
LCFI29:
	ret
LFE5971:
	.cstring
lC3:
	.ascii "lib/opensvcs\0"
lC4:
	.ascii "opensvc_\0"
	.text
__ZL14subinfo_startsP7subinfo:
LFB5972:
	pushq	%rbp
LCFI30:
	movq	%rsp, %rbp
LCFI31:
	subq	$1520, %rsp
	movq	%rdi, -1512(%rbp)
	movq	-1512(%rbp), %rax
	movq	96(%rax), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rcx
	leaq	-1088(%rbp), %rax
	leaq	lC3(%rip), %rdx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	_mkpath2
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L27
	movl	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	leaq	-1504(%rbp), %rdx
	leaq	-1088(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_u_stat
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L27
	movzwl	-1500(%rbp), %eax
	movzwl	%ax, %eax
	andl	$61440, %eax
	cmpl	$16384, %eax
	jne	L29
	leaq	-1088(%rbp), %rax
	movq	%rax, -32(%rbp)
	leaq	-1096(%rbp), %rax
	movl	-20(%rbp), %esi
	movq	-32(%rbp), %rcx
	movq	%rax, %rdx
	movq	%rcx, %rdi
	call	_uc_libmallocstrw
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L27
	movl	$255, -36(%rbp)
	leaq	lC4(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	-1512(%rbp), %rax
	movq	112(%rax), %rax
	movq	%rax, -56(%rbp)
	movq	-1096(%rbp), %rdx
	movq	-1512(%rbp), %rax
	movq	%rdx, 120(%rax)
	movq	-56(%rbp), %rcx
	movq	-48(%rbp), %rdx
	leaq	-1360(%rbp), %rax
	movl	$255, %esi
	movq	%rax, %rdi
	call	_sncpy2
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L31
	leaq	-1096(%rbp), %rdx
	movl	-4(%rbp), %ecx
	leaq	-1360(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	_uc_libmallocstrw
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L31
	movq	-1096(%rbp), %rdx
	movq	-1512(%rbp), %rax
	movq	%rdx, 128(%rax)
L31:
	cmpl	$0, -4(%rbp)
	jns	L27
	movq	-1512(%rbp), %rax
	movq	120(%rax), %rax
	movq	%rax, %rdi
	call	_uc_libfree
	movq	-1512(%rbp), %rax
	movq	$0, 120(%rax)
	jmp	L27
L29:
	movl	$-20, -4(%rbp)
L27:
	movl	-4(%rbp), %eax
	leave
LCFI32:
	ret
LFE5972:
__ZL14subinfo_finishP7subinfo:
LFB5973:
	pushq	%rbp
LCFI33:
	movq	%rsp, %rbp
LCFI34:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -4(%rbp)
	movq	-24(%rbp), %rax
	movq	128(%rax), %rax
	testq	%rax, %rax
	je	L35
	movq	-24(%rbp), %rax
	movq	128(%rax), %rax
	movq	%rax, %rdi
	call	_uc_libfree
	movl	%eax, -8(%rbp)
	cmpl	$0, -4(%rbp)
	js	L36
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
L36:
	movq	-24(%rbp), %rax
	movq	$0, 128(%rax)
L35:
	movq	-24(%rbp), %rax
	movq	120(%rax), %rax
	testq	%rax, %rax
	je	L37
	movq	-24(%rbp), %rax
	movq	120(%rax), %rax
	movq	%rax, %rdi
	call	_uc_libfree
	movl	%eax, -8(%rbp)
	cmpl	$0, -4(%rbp)
	js	L38
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
L38:
	movq	-24(%rbp), %rax
	movq	$0, 120(%rax)
L37:
	movl	-4(%rbp), %eax
	leave
LCFI35:
	ret
LFE5973:
__ZL14subinfo_searchP7subinfo:
LFB5974:
	pushq	%rbp
LCFI36:
	movq	%rsp, %rbp
LCFI37:
	subq	$224, %rsp
	movq	%rdi, -216(%rbp)
	movl	$1024, -12(%rbp)
	movl	$0, -8(%rbp)
	movq	-216(%rbp), %rax
	movq	%rax, %rdi
	call	__ZL15subinfo_idbeginP7subinfo
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L41
	movl	$0, -16(%rbp)
	movl	$2050, -20(%rbp)
	leaq	-64(%rbp), %rax
	movq	%rax, %rsi
	movl	$2050, %edi
	call	_uc_libmalloc
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L42
	movq	-216(%rbp), %rax
	movq	96(%rax), %rax
	movq	%rax, -32(%rbp)
	movq	-64(%rbp), %rcx
	movl	-16(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -16(%rbp)
	movl	%eax, %edx
	sall	$10, %edx
	addl	%edx, %eax
	cltq
	addq	%rcx, %rax
	movq	%rax, -40(%rbp)
	movq	-64(%rbp), %rcx
	movl	-16(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -16(%rbp)
	movl	%eax, %edx
	sall	$10, %edx
	addl	%edx, %eax
	cltq
	addq	%rcx, %rax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rcx
	movq	-40(%rbp), %rax
	leaq	lC3(%rip), %rdx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	_mkpath2
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L43
	leaq	-208(%rbp), %rax
	movq	-40(%rbp), %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	_u_stat
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L44
	movzwl	-204(%rbp), %eax
	movzwl	%ax, %eax
	andl	$61440, %eax
	cmpl	$16384, %eax
	jne	L43
	movq	-48(%rbp), %rdx
	movq	-40(%rbp), %rcx
	movq	-216(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	__ZL12subinfo_extsP7subinfoPKcPc
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	L43
L44:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	__ZL7isNoAcci
	testb	%al, %al
	je	L43
	movl	$0, -4(%rbp)
L43:
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	_uc_libfree
	movl	%eax, -52(%rbp)
	cmpl	$0, -4(%rbp)
	js	L42
	movl	-52(%rbp), %eax
	movl	%eax, -4(%rbp)
L42:
	movq	-216(%rbp), %rax
	movq	%rax, %rdi
	call	__ZL13subinfo_idendP7subinfo
	movl	%eax, -52(%rbp)
	cmpl	$0, -4(%rbp)
	js	L41
	movl	-52(%rbp), %eax
	movl	%eax, -4(%rbp)
L41:
	cmpl	$0, -4(%rbp)
	jns	L46
	movq	-216(%rbp), %rax
	movl	164(%rax), %eax
	testl	%eax, %eax
	js	L46
	movq	-216(%rbp), %rax
	movl	164(%rax), %eax
	movl	%eax, %edi
	call	_u_close
	movq	-216(%rbp), %rax
	movl	$-1, 164(%rax)
L46:
	cmpl	$0, -4(%rbp)
	js	L47
	movl	-8(%rbp), %eax
	jmp	L49
L47:
	movl	-4(%rbp), %eax
L49:
	leave
LCFI38:
	ret
LFE5974:
__ZL12subinfo_extsP7subinfoPKcPc:
LFB5975:
	pushq	%rbp
LCFI39:
	movq	%rsp, %rbp
LCFI40:
	subq	$208, %rsp
	movq	%rdi, -184(%rbp)
	movq	%rsi, -192(%rbp)
	movq	%rdx, -200(%rbp)
	movl	$5, -16(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	movq	-184(%rbp), %rax
	movq	112(%rax), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -12(%rbp)
	jmp	L51
L59:
	movl	-12(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	__ZL6soexts(%rip), %rax
	movq	(%rdx,%rax), %rcx
	movq	-24(%rbp), %rdx
	movq	-192(%rbp), %rsi
	movq	-200(%rbp), %rax
	movq	%rax, %rdi
	call	_mksofname
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L52
	leaq	-176(%rbp), %rax
	movq	-200(%rbp), %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	_u_stat
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L53
	movzwl	-172(%rbp), %eax
	movzwl	%ax, %eax
	andl	$61440, %eax
	cmpl	$32768, %eax
	jne	L52
	movq	-184(%rbp), %rcx
	leaq	-176(%rbp), %rax
	movl	$5, %edx
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	_sperm
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L55
	movq	-200(%rbp), %rdx
	movq	-184(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	__ZL17subinfo_searchlibP7subinfoPKc
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	L52
L55:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	__ZL7isNoAcci
	testb	%al, %al
	je	L52
	movl	$0, -4(%rbp)
	jmp	L52
L53:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	_isNotPresent
	testb	%al, %al
	je	L52
	movl	$0, -4(%rbp)
L52:
	cmpl	$0, -8(%rbp)
	jne	L63
	cmpl	$0, -4(%rbp)
	js	L64
	addl	$1, -12(%rbp)
L51:
	movl	-12(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	__ZL6soexts(%rip), %rax
	movq	(%rdx,%rax), %rax
	testq	%rax, %rax
	jne	L59
	jmp	L57
L63:
	nop
	jmp	L57
L64:
	nop
L57:
	cmpl	$0, -4(%rbp)
	js	L60
	movl	-8(%rbp), %eax
	jmp	L62
L60:
	movl	-4(%rbp), %eax
L62:
	leave
LCFI41:
	ret
LFE5975:
__ZL17subinfo_searchlibP7subinfoPKc:
LFB5976:
	pushq	%rbp
LCFI42:
	movq	%rsp, %rbp
LCFI43:
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	movq	-48(%rbp), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	_dlopen
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	setne	%al
	testb	%al, %al
	je	L66
	movq	-40(%rbp), %rax
	movq	128(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_dlsym
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	setne	%al
	testb	%al, %al
	je	L67
	movq	-32(%rbp), %rdx
	movq	-40(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	__ZL18subinfo_searchcallP7subinfoPDoFiPKcS2_itPKS2_S4_iE
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
L67:
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	_dlclose
L66:
	cmpl	$0, -4(%rbp)
	js	L68
	movl	-8(%rbp), %eax
	jmp	L70
L68:
	movl	-4(%rbp), %eax
L70:
	leave
LCFI44:
	ret
LFE5976:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table0:
LLSDA5976:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE5976-LLSDACSB5976
LLSDACSB5976:
LLSDACSE5976:
	.text
__ZL18subinfo_searchcallP7subinfoPDoFiPKcS2_itPKS2_S4_iE:
LFB5977:
	pushq	%rbp
LCFI45:
	movq	%rsp, %rbp
LCFI46:
	pushq	%rbx
	subq	$88, %rsp
LCFI47:
	movq	%rdi, -88(%rbp)
	movq	%rsi, -96(%rbp)
	movq	-88(%rbp), %rax
	movl	156(%rax), %eax
	movl	%eax, -24(%rbp)
	movq	-88(%rbp), %rax
	movl	160(%rax), %eax
	movl	%eax, -28(%rbp)
	movq	-88(%rbp), %rax
	movzwl	152(%rax), %eax
	movw	%ax, -30(%rbp)
	movl	$0, -20(%rbp)
	movq	-88(%rbp), %rax
	movq	96(%rax), %rax
	movq	%rax, -40(%rbp)
	movq	-88(%rbp), %rax
	movq	104(%rax), %rax
	movq	%rax, -48(%rbp)
	movq	-88(%rbp), %rax
	movq	136(%rax), %rax
	movq	%rax, -56(%rbp)
	movq	-88(%rbp), %rax
	movq	144(%rax), %rax
	movq	%rax, -64(%rbp)
	movq	-96(%rbp), %rbx
	movzwl	-30(%rbp), %eax
	movq	-64(%rbp), %r9
	movq	-56(%rbp), %r8
	movl	-24(%rbp), %edx
	movq	-48(%rbp), %rsi
	movq	-40(%rbp), %rdi
	subq	$8, %rsp
	movl	-28(%rbp), %ecx
	pushq	%rcx
	movl	%eax, %ecx
	call	*%rbx
	addq	$16, %rsp
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L72
	movq	-88(%rbp), %rax
	movl	-68(%rbp), %edx
	movl	%edx, 164(%rax)
	movl	$1, -20(%rbp)
L72:
	cmpl	$0, -68(%rbp)
	js	L73
	movl	-20(%rbp), %eax
	jmp	L75
L73:
	movl	-68(%rbp), %eax
L75:
	movq	-8(%rbp), %rbx
	leave
LCFI48:
	ret
LFE5977:
__ZL15subinfo_idbeginP7subinfo:
LFB5978:
	pushq	%rbp
LCFI49:
	movq	%rsp, %rbp
LCFI50:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	_ids_load
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	leave
LCFI51:
	ret
LFE5978:
__ZL13subinfo_idendP7subinfo:
LFB5979:
	pushq	%rbp
LCFI52:
	movq	%rsp, %rbp
LCFI53:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -4(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	_ids_release
	movl	%eax, -8(%rbp)
	cmpl	$0, -4(%rbp)
	js	L79
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
L79:
	movl	-4(%rbp), %eax
	leave
LCFI54:
	ret
LFE5979:
	.align 1,0x90
__ZN12_GLOBAL__N_14varscviEv:
LFB5980:
	pushq	%rbp
LCFI55:
	movq	%rsp, %rbp
LCFI56:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$2, %edi
	call	_getbufsize
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L82
	movq	-24(%rbp), %rax
	movl	-4(%rbp), %edx
	movl	%edx, (%rax)
	movl	$3, %edi
	call	_getbufsize
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	shrl	$31, %eax
	testb	%al, %al
	je	L82
	movq	-24(%rbp), %rax
	movl	-4(%rbp), %edx
	movl	%edx, 4(%rax)
L82:
	movl	-4(%rbp), %eax
	leave
LCFI57:
	ret
LFE5980:
__ZL7isNoAcci:
LFB5981:
	pushq	%rbp
LCFI58:
	movq	%rsp, %rbp
LCFI59:
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	__ZL7rsnoacc(%rip), %rax
	movq	%rax, %rdi
	call	_isOneOf
	leave
LCFI60:
	ret
LFE5981:
	.globl __Z8memclearI7subinfoEiPT_
	.weak_definition __Z8memclearI7subinfoEiPT_
__Z8memclearI7subinfoEiPT_:
LFB6625:
	pushq	%rbp
LCFI61:
	movq	%rsp, %rbp
LCFI62:
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$168, -4(%rbp)
	movl	-4(%rbp), %edx
	movq	-24(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	__Z8memclearPvi
	leave
LCFI63:
	ret
LFE6625:
	.const
__ZNSt8__detail30__integer_to_chars_is_unsignedIjEE:
	.byte	1
__ZNSt8__detail30__integer_to_chars_is_unsignedImEE:
	.byte	1
__ZNSt8__detail30__integer_to_chars_is_unsignedIyEE:
	.byte	1
__ZSt39__is_implicitly_default_constructible_vItE:
	.byte	1
__ZSt39__is_implicitly_default_constructible_vIPKcE:
	.byte	1
__ZNSt8__detail14__destructibleIN9__gnu_cxx17__normal_iteratorIPcSt4spanIcLm18446744073709551615EEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implIN9__gnu_cxx17__normal_iteratorIPcSt4spanIcLm18446744073709551615EEEEEE:
	.byte	1
__ZNSt8__detail14__destructibleISt16reverse_iteratorIN9__gnu_cxx17__normal_iteratorIPcSt4spanIcLm18446744073709551615EEEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implISt16reverse_iteratorIN9__gnu_cxx17__normal_iteratorIPcSt4spanIcLm18446744073709551615EEEEEEE:
	.byte	1
__ZNSt8__detail14__destructibleINSt8__format10_Sink_iterIcEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implINSt8__format10_Sink_iterIcEEEE:
	.byte	1
__ZNSt8__detail14__destructibleIN9__gnu_cxx17__normal_iteratorIPwSt4spanIwLm18446744073709551615EEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implIN9__gnu_cxx17__normal_iteratorIPwSt4spanIwLm18446744073709551615EEEEEE:
	.byte	1
__ZNSt8__detail14__destructibleISt16reverse_iteratorIN9__gnu_cxx17__normal_iteratorIPwSt4spanIwLm18446744073709551615EEEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implISt16reverse_iteratorIN9__gnu_cxx17__normal_iteratorIPwSt4spanIwLm18446744073709551615EEEEEEE:
	.byte	1
__ZNSt8__detail14__destructibleINSt8__format10_Sink_iterIwEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implINSt8__format10_Sink_iterIwEEEE:
	.byte	1
__ZNSt8__detail14__destructibleIPcEE:
	.byte	1
__ZNSt8__detail19__destructible_implIPcEE:
	.byte	1
__ZNSt8__detail14__destructibleIPwEE:
	.byte	1
__ZNSt8__detail19__destructible_implIPwEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1ELl1000EEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1000000000ELl1EEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1ELl1000000EEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1ELl1000000000EEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1ELl1EEE:
	.byte	1
__ZNSt8__detail14__destructibleIN9__gnu_cxx17__normal_iteratorIPcNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implIN9__gnu_cxx17__normal_iteratorIPcNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEEE:
	.byte	1
__ZNSt8__detail30__integer_to_chars_is_unsignedIoEE:
	.byte	1
__ZSt22__is_specialization_ofINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES1_E:
	.byte	1
__ZSt22__is_specialization_ofINSt7__cxx1112basic_stringIwSt11char_traitsIwESaIwEEES1_E:
	.byte	1
__ZNSt8__detail14__destructibleImEE:
	.byte	1
__ZNSt8__detail19__destructible_implImEE:
	.byte	1
__ZSt12__is_ratio_vISt5ratioILl1000ELl1EEE:
	.byte	1
__ZSt39__is_implicitly_default_constructible_vIPKwE:
	.byte	1
__ZNSt8__detail14__destructibleIPKcEE:
	.byte	1
__ZNSt8__detail19__destructible_implIPKcEE:
	.byte	1
__ZNSt8__detail14__destructibleIPKwEE:
	.byte	1
__ZNSt8__detail19__destructible_implIPKwEE:
	.byte	1
__ZSt22__is_specialization_ofINSt7__cxx1112basic_stringIwSt11char_traitsIwESaIwEEESt6vectorE:
	.space 1
__ZNSt8__detail14__destructibleIN9__gnu_cxx17__normal_iteratorIPwNSt7__cxx1112basic_stringIwSt11char_traitsIwESaIwEEEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implIN9__gnu_cxx17__normal_iteratorIPwNSt7__cxx1112basic_stringIwSt11char_traitsIwESaIwEEEEEEE:
	.byte	1
__ZSt22__is_specialization_ofINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt6vectorE:
	.space 1
__ZNSt8__detail14__destructibleIiEE:
	.byte	1
__ZNSt8__detail19__destructible_implIiEE:
	.byte	1
__ZNSt8__detail14__destructibleIjEE:
	.byte	1
__ZNSt8__detail19__destructible_implIjEE:
	.byte	1
__ZNSt8__detail14__destructibleIlEE:
	.byte	1
__ZNSt8__detail19__destructible_implIlEE:
	.byte	1
__ZNSt8__detail14__destructibleIPKDiEE:
	.byte	1
__ZNSt8__detail19__destructible_implIPKDiEE:
	.byte	1
__ZNSt8__detail14__destructibleINSt9__unicode13_Utf_iteratorIDicPKDiS4_NS1_5_ReplEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implINSt9__unicode13_Utf_iteratorIDicPKDiS4_NS1_5_ReplEEEEE:
	.byte	1
__ZNSt8__detail14__destructibleISt17basic_string_viewIcSt11char_traitsIcEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implISt17basic_string_viewIcSt11char_traitsIcEEEE:
	.byte	1
__ZNSt8__detail14__destructibleINSt9__unicode13_Utf_iteratorIcDiPKcS4_NS1_5_ReplEEEEE:
	.byte	1
__ZNSt8__detail19__destructible_implINSt9__unicode13_Utf_iteratorIcDiPKcS4_NS1_5_ReplEEEEE:
	.byte	1
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x3
	.ascii "zPLR\0"
	.uleb128 0x1
	.sleb128 -8
	.uleb128 0x10
	.uleb128 0x7
	.byte	0x9b
	.long	___gxx_personality_v0+4@GOTPCREL
	.byte	0x10
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB5552-.
	.set L$set$2,LFE5552-LFB5552
	.quad L$set$2
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB5552
	.long L$set$3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE1:
LSFDE3:
	.set L$set$6,LEFDE3-LASFDE3
	.long L$set$6
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB5900-.
	.set L$set$7,LFE5900-LFB5900
	.quad L$set$7
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$8,LCFI3-LFB5900
	.long L$set$8
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$9,LCFI4-LCFI3
	.long L$set$9
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$10,LCFI5-LCFI4
	.long L$set$10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE3:
LSFDE5:
	.set L$set$11,LEFDE5-LASFDE5
	.long L$set$11
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB5905-.
	.set L$set$12,LFE5905-LFB5905
	.quad L$set$12
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$13,LCFI6-LFB5905
	.long L$set$13
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$14,LCFI7-LCFI6
	.long L$set$14
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$15,LCFI8-LCFI7
	.long L$set$15
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE5:
LSFDE7:
	.set L$set$16,LEFDE7-LASFDE7
	.long L$set$16
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB5907-.
	.set L$set$17,LFE5907-LFB5907
	.quad L$set$17
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$18,LCFI9-LFB5907
	.long L$set$18
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$19,LCFI10-LCFI9
	.long L$set$19
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$20,LCFI11-LCFI10
	.long L$set$20
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE7:
LSFDE9:
	.set L$set$21,LEFDE9-LASFDE9
	.long L$set$21
LASFDE9:
	.long	LASFDE9-EH_frame1
	.quad	LFB5910-.
	.set L$set$22,LFE5910-LFB5910
	.quad L$set$22
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$23,LCFI12-LFB5910
	.long L$set$23
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$24,LCFI13-LCFI12
	.long L$set$24
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$25,LCFI14-LCFI13
	.long L$set$25
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE9:
LSFDE11:
	.set L$set$26,LEFDE11-LASFDE11
	.long L$set$26
LASFDE11:
	.long	LASFDE11-EH_frame1
	.quad	LFB5957-.
	.set L$set$27,LFE5957-LFB5957
	.quad L$set$27
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$28,LCFI15-LFB5957
	.long L$set$28
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$29,LCFI16-LCFI15
	.long L$set$29
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$30,LCFI17-LCFI16
	.long L$set$30
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE11:
LSFDE13:
	.set L$set$31,LEFDE13-LASFDE13
	.long L$set$31
LASFDE13:
	.long	LASFDE13-EH_frame1
	.quad	LFB5967-.
	.set L$set$32,LFE5967-LFB5967
	.quad L$set$32
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$33,LCFI18-LFB5967
	.long L$set$33
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$34,LCFI19-LCFI18
	.long L$set$34
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$35,LCFI20-LCFI19
	.long L$set$35
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE13:
LSFDE15:
	.set L$set$36,LEFDE15-LASFDE15
	.long L$set$36
LASFDE15:
	.long	LASFDE15-EH_frame1
	.quad	LFB5970-.
	.set L$set$37,LFE5970-LFB5970
	.quad L$set$37
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$38,LCFI21-LFB5970
	.long L$set$38
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$39,LCFI22-LCFI21
	.long L$set$39
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$40,LCFI23-LCFI22
	.long L$set$40
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE15:
LSFDE17:
	.set L$set$41,LEFDE17-LASFDE17
	.long L$set$41
LASFDE17:
	.long	LASFDE17-EH_frame1
	.quad	LFB5964-.
	.set L$set$42,LFE5964-LFB5964
	.quad L$set$42
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$43,LCFI24-LFB5964
	.long L$set$43
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$44,LCFI25-LCFI24
	.long L$set$44
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$45,LCFI26-LCFI25
	.long L$set$45
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE17:
LSFDE19:
	.set L$set$46,LEFDE19-LASFDE19
	.long L$set$46
LASFDE19:
	.long	LASFDE19-EH_frame1
	.quad	LFB5971-.
	.set L$set$47,LFE5971-LFB5971
	.quad L$set$47
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$48,LCFI27-LFB5971
	.long L$set$48
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$49,LCFI28-LCFI27
	.long L$set$49
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$50,LCFI29-LCFI28
	.long L$set$50
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE19:
LSFDE21:
	.set L$set$51,LEFDE21-LASFDE21
	.long L$set$51
LASFDE21:
	.long	LASFDE21-EH_frame1
	.quad	LFB5972-.
	.set L$set$52,LFE5972-LFB5972
	.quad L$set$52
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$53,LCFI30-LFB5972
	.long L$set$53
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$54,LCFI31-LCFI30
	.long L$set$54
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$55,LCFI32-LCFI31
	.long L$set$55
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE21:
LSFDE23:
	.set L$set$56,LEFDE23-LASFDE23
	.long L$set$56
LASFDE23:
	.long	LASFDE23-EH_frame1
	.quad	LFB5973-.
	.set L$set$57,LFE5973-LFB5973
	.quad L$set$57
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$58,LCFI33-LFB5973
	.long L$set$58
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$59,LCFI34-LCFI33
	.long L$set$59
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$60,LCFI35-LCFI34
	.long L$set$60
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE23:
LSFDE25:
	.set L$set$61,LEFDE25-LASFDE25
	.long L$set$61
LASFDE25:
	.long	LASFDE25-EH_frame1
	.quad	LFB5974-.
	.set L$set$62,LFE5974-LFB5974
	.quad L$set$62
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$63,LCFI36-LFB5974
	.long L$set$63
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$64,LCFI37-LCFI36
	.long L$set$64
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$65,LCFI38-LCFI37
	.long L$set$65
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE25:
LSFDE27:
	.set L$set$66,LEFDE27-LASFDE27
	.long L$set$66
LASFDE27:
	.long	LASFDE27-EH_frame1
	.quad	LFB5975-.
	.set L$set$67,LFE5975-LFB5975
	.quad L$set$67
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$68,LCFI39-LFB5975
	.long L$set$68
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$69,LCFI40-LCFI39
	.long L$set$69
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$70,LCFI41-LCFI40
	.long L$set$70
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE27:
LSFDE29:
	.set L$set$71,LEFDE29-LASFDE29
	.long L$set$71
LASFDE29:
	.long	LASFDE29-EH_frame1
	.quad	LFB5976-.
	.set L$set$72,LFE5976-LFB5976
	.quad L$set$72
	.uleb128 0x8
	.quad	LLSDA5976-.
	.byte	0x4
	.set L$set$73,LCFI42-LFB5976
	.long L$set$73
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$74,LCFI43-LCFI42
	.long L$set$74
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$75,LCFI44-LCFI43
	.long L$set$75
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE29:
LSFDE31:
	.set L$set$76,LEFDE31-LASFDE31
	.long L$set$76
LASFDE31:
	.long	LASFDE31-EH_frame1
	.quad	LFB5977-.
	.set L$set$77,LFE5977-LFB5977
	.quad L$set$77
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$78,LCFI45-LFB5977
	.long L$set$78
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$79,LCFI46-LCFI45
	.long L$set$79
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$80,LCFI47-LCFI46
	.long L$set$80
	.byte	0x83
	.uleb128 0x3
	.byte	0x4
	.set L$set$81,LCFI48-LCFI47
	.long L$set$81
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE31:
LSFDE33:
	.set L$set$82,LEFDE33-LASFDE33
	.long L$set$82
LASFDE33:
	.long	LASFDE33-EH_frame1
	.quad	LFB5978-.
	.set L$set$83,LFE5978-LFB5978
	.quad L$set$83
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$84,LCFI49-LFB5978
	.long L$set$84
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$85,LCFI50-LCFI49
	.long L$set$85
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$86,LCFI51-LCFI50
	.long L$set$86
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE33:
LSFDE35:
	.set L$set$87,LEFDE35-LASFDE35
	.long L$set$87
LASFDE35:
	.long	LASFDE35-EH_frame1
	.quad	LFB5979-.
	.set L$set$88,LFE5979-LFB5979
	.quad L$set$88
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$89,LCFI52-LFB5979
	.long L$set$89
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$90,LCFI53-LCFI52
	.long L$set$90
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$91,LCFI54-LCFI53
	.long L$set$91
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE35:
LSFDE37:
	.set L$set$92,LEFDE37-LASFDE37
	.long L$set$92
LASFDE37:
	.long	LASFDE37-EH_frame1
	.quad	LFB5980-.
	.set L$set$93,LFE5980-LFB5980
	.quad L$set$93
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$94,LCFI55-LFB5980
	.long L$set$94
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$95,LCFI56-LCFI55
	.long L$set$95
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$96,LCFI57-LCFI56
	.long L$set$96
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE37:
LSFDE39:
	.set L$set$97,LEFDE39-LASFDE39
	.long L$set$97
LASFDE39:
	.long	LASFDE39-EH_frame1
	.quad	LFB5981-.
	.set L$set$98,LFE5981-LFB5981
	.quad L$set$98
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$99,LCFI58-LFB5981
	.long L$set$99
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$100,LCFI59-LCFI58
	.long L$set$100
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$101,LCFI60-LCFI59
	.long L$set$101
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE39:
LSFDE41:
	.set L$set$102,LEFDE41-LASFDE41
	.long L$set$102
LASFDE41:
	.long	LASFDE41-EH_frame1
	.quad	LFB6625-.
	.set L$set$103,LFE6625-LFB6625
	.quad L$set$103
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$104,LCFI61-LFB6625
	.long L$set$104
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.set L$set$105,LCFI62-LCFI61
	.long L$set$105
	.byte	0xd
	.uleb128 0x6
	.byte	0x4
	.set L$set$106,LCFI63-LCFI62
	.long L$set$106
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.align 3
LEFDE41:
	.ident	"GCC: (MacPorts gcc14 14.2.0_1+stdlib_flag) 14.2.0"
	.subsections_via_symbols
