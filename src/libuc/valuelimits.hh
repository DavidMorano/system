/* valuelimits HEADER */
/* lang=C++20 */

/* provide value limits on the various integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VALUELIMITS_INCLUDE
#define	VALUELIMITS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


struct valuelimits {
	short		smin = short(0x8000) ;
	short		smax = short(0x7FFF) ;
	int		imin = INT_MIN ;
	int		imax = INT_MAX ;
	long		lmin = LONG_MIN ;
	long		lmax = LONG_MAX ;
	longlong	llmin = 0 ;
	longlong	llmax = 0 ;
	ushort		usmax = ushort(0xFFFF) ;
	uint		uimax = UINT_MAX ;
	ulong		ulmax = ULONG_MAX ;
	ulonglong	ullmax = 0 ;
	constexpr void mklonglong () noex {
	    const longlong	one = 1 ;
	    cint	n = (8*sizeof(longlong)) ;
	    ullmax = ~ullmax ;
	    llmin = (one << (n-1)) ;
	    llmax = longlong(ullmax >> 1) ;
	} ;
	constexpr valuelimits() noex {
	    mklonglong() ;
	} ;
} ; /* end subroutine (valuelimits) */


#endif /* __cplusplus */
#endif /* VALUELIMITS_INCLUDE */


