/* cfdigx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written by being adapted from one of
	my previous versions of the same (from the early 1980s;
	embedded work at AT&T), which itself was adapted from an
	original asembly-language version (in Digital Equipment Corp
	VAX assembly).

	= 2013-04-30, David A­D­ Morano
	I took the plunge and rewrote this set of subroutines using
	the LIBC subroutines |strtoXX(3c)|, being much simplified
	now.  It is no longer stand-alone (no LIBC dependency),
	like in the old days, but we have been mostly on UNIX®i for
	some time now (decades) and use in non-UNIX®i environments
	is now quite rare.  I hope that this is not a problem.  We
	will see.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

	= 2023-10,10, David A-D- Morano
	I took the liberty to rewite these subroutines in terms of
	a subroutine-template.  This did nothing to make the code
	faster in any way.  After template expansion the code is the
	same as before.  All type-versions of the former subroutines
	are now in a single subroutine-template.  I guess that I had
	too much idle time on my hands.

*/

/* Copyright © 1998,2013,2020,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	cfdigsx		(si-signed)
	cfdigux		(un-signed)

	Description:
	These are subroutine templates to convert digit strings
	into binary integers (of various types and sizes).  All
	number bases from 2 through 64 are supported.  The integer
	types |int|, |long|, and |longlong)| along with their
	unsigned versions are supported.

	Synopsis:
    	template<typename ST> 
	inline int cfdigsx(cc *sp,int sl,int b,ST *rp) noex
    	template<typename UT> 
	inline int cfdigux(cc *sp,int sl,int b,UT *rp) noex

	Arguments:
	sp		digit c-string pointer (to convert to integer)
	sl		digit c-string length  (to convert to integer)
	b		number base
	rp		pinter to hold result (of the given type)

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	CFDIGX_INCLUDE
#define	CFDIGX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<envstandards.h>	/* MUST be first to configure */


#endif /* CFDIGX_INCLUDE */


