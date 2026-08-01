/* conallof SUPPORT (contains-all-of) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given string have all of the given characters? */
/* version %I% last-modified %G% */

#define	CF_BITSET	1		/* compile in alternative version */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	conallof

	Description:
	Determine if the given string has all of the specified
	characters within it.

	Synopsis:
	int conallof(cchar *sp,int sl,cchar *tstr) noex

	Arguments:
	sp		source string to test (pointer)
	sl		source string to test (length)
	tstr		c-string of characters to test against

	Returns:
	>0		yes, the string has all of the characters specified
	==0		no, did not have all characters specified
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bitset>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<dupstr.h>		/* LIBUC */
#include	<six.h>			/* LIBUC |sichr(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"conallof.h"


/* local defines */

#ifndef	CF_BITSET
#define	CF_BITSET	1		/* compile in alternative version */
#endif


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int conallof_bitset(cchar *,int,cchar *) noex ;
local int conallof_dupstr(cchar *,int,cchar *) noex ;


/* local variables */

cint		nchars		= (UCHAR_MAX + 1) ;
cbool		f_bitset	= CF_BITSET ;


/* exported variables */


/* exported subroutines */

int conallof(cchar *sp,int sl,cchar *tstr) noex {
	int		rs = SR_FAULT ;
	int		f = true ;
	if (sp && tstr) {
	    rs = SR_INVALID ;
	    if (tstr[0]) {
		if_constexpr (f_bitset) {
		    rs = conallof_bitset(sp,sl,tstr) ;
		    f = rs ;
		} else {
		    rs = conallof_dupstr(sp,sl,tstr) ;
		    f = rs ;
		} /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (conallof) */


/* local subroutines */

namespace {
    struct bitsetx : bitset<nchars> {
	int load(cchar *) noex ;
    } ; /* end struct (bitsetx) */
} /* end namespace */

local int conallof_bitset(cchar *sp,int sl,cchar *tstr) noex {
	int		rs ;
	int		f = false ;
	if (bitsetx bs ; (rs = bs.load(tstr)) >= 0) {
	    for (int ch ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
	 	if (bs[ch]) {
		    bs.reset(ch) ;
		    f = bs.none() ;
	            if (f) break ;
		} /* end if */
	    } /* end for */
	} /* end if (bitsetx) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (conallof) */

local int conallof_dupstr(cchar *sp,int sl,cchar *tstr) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	char		*bp{} ;
	if (dupstr sd ; (rs = sd.start(tstr,-1,&bp)) >= 0) {
	    for (int bl = rs ; sl-- && *sp ; sp += 1) {
		if (int si ; (si = sichr(bp,bl,sp[0])) >= 0) {
		    if (bl-- > 1) {
			if (si < bl) bp[si] = bp[bl] ;
		    }
		    f = (bl == 0) ;
	            if (f) break ;
		} /* end if (sichr) */
	    } /* end for */
	    rs1 = sd.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dupstr) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (conallof_dupstr) */

int bitsetx::load(cchar *sp) noex {
	int		rs = SR_OK ;
	try {
	    for (int ch ; (ch = mkchar(*sp)) > 0 ; sp += 1) {
		set(ch) ;
	    } /* end for */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
} /* end method (bitsetx::load) */


