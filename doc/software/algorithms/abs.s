; abs SUPPORT
; lang=MC68x00-assembly
; encoding=ISO8859-1

; take and return the absolute value of a signed integer
; version %I% last-modified %G%


; revision history:

	= 1985-04-17, David A-D- Morano
	This subroutine was originally written.

; ****

; Copyright © 1985 David A­D­ Morano.  All rights reserved.

;*******************************************************************************

	Name:
	abs

	Description:
	This subroutine is an (extremely) optimixed version of the 
	similar subroutine |abs(3c)| in the C-language standard library.

	Synopsis:
	int abs(int v) noex

	Aruguments:
	v		signed value to absolute value of

	Returns:
	-		absolute value of caller passed signed value

;*******************************************************************************


; exported subroutines

; subroutine begin (abs)
	global	abs
abs:
	movl	4(%sp),%d0
	movl	%d0,%d1
	addl	%d1,%d1
	subxl	%d1,%d1
	eorl	%d1,%d0
	subl	%d1,%d0
	rts
; subroutine end (abs)


