/* valuelims HEADER */
/* lang=C++20 */

/* provide value limits on the various integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	This object provides various value limits on various integer
	types.

*******************************************************************************/

#ifndef	VALUELIMS_INCLUDE
#define	VALUELIMS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| + others */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


struct valuelims {
	cshort		smin = short(0x8000) ;
	cshort		smax = short(0x7FFF) ;
	cint		imin = INT_MIN ;
	cint		imax = INT_MAX ;
	clong		lmin = LONG_MIN ;
	clong		lmax = LONG_MAX ;
	longlong	llmin = 0 ;
	longlong	llmax = 0 ;
	cushort		usmax = ushort(0xFFFF) ;
	cuint		uimax = UINT_MAX ;
	culong		ulmax = ULONG_MAX ;
	ulonglong	ullmax = 0 ;
	constexpr void mklonglong () noex {
	    clonglong	one = 1 ;
	    cint	n = int(CHAR_BIT * sizeof(longlong)) ;
	    ullmax = ~ullmax ;
	    llmin = (one << (n - 1)) ;
	    llmax = longlong(ullmax >> 1) ;
	} ;
	constexpr valuelims() noex {
	    mklonglong() ;
	} ;
} ; /* end subroutine (valuelims) */


#endif /* __cplusplus */
#endif /* VALUELIMS_INCLUDE */


