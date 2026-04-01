/* cfpowx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* conversion of power-base-two c-string to their binary integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-05-06, David A­D­ Morano
	Convert a numeric digit c-string (of a power-of-two base) into
	an integer (type).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2008,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cfpowsx
	cfpowux

	Description:
	This code converts a c-string of digits (of a power-of-two
	base) into the normal integer types: these being |int|,
	|long|, and |longlong| and their associated unsigned
	variations.  Power-of-two bases (up to a maximum of 64)
	are: 2, 4, 8, 16, 32, 64.

	Notes:
	1. This code is limited to a maximum base of 64 (the standard).
	2. I find it (sort of) funny that I wrote a subroutine that
	does this very function back in 1983 (37 years ago; it is
	2020), but in M68k assembly language!

*******************************************************************************/

#ifndef	CFPOWX_INCLUDE
#define	CFPOWX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<envstandards.h>	/* ordered first to configure */


#endif /* CFPOWX_INCLUDE */


