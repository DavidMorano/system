/* cfalphax HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* conversion of c-string digits into binary integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-05-19, David A­D­ Morano
	Convert numeric digit c-strings into integers.  This code
	replaces a previous algorithm that used to be used for this
	function.  I think that I was on drugs when I wrote that
	previous version (but people might say the same about this
	version also).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2008,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cfalphax

	Description:
	This subroutine (template) converts a c-string of
	digits consisting of just alphabetic-type characters
	(characters 'A' -- 'Z'), along with a number base (from
	2 to 26) into the normal integer types: these being |int|,
	|long|, and |longlong| and their associated unsigned
	variations.  The conversion is for signed numbers only.  I
	am considering only signed numbers for these Alpha-base
	numbers.

	Synopsis:
    	template<typename T> 
	inline int cfalphax(cc *sp,int sl,int b,T *rp) noex

	Arguments:
	sp		source c-string od digits pointer
	sl		source c-string od digits length
	b		number base to use (2-26)
	rp		result value (of the given type) pointer

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	1. This code is limited to a maximum base of 64.
	2. This code only handles signed numbers.

*******************************************************************************/

#ifndef	CFALPHAX_INCLUDE
#define	CFALPHAX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#endif /* CFALPHAX_INCLUDE */


