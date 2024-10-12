/* conallof SUPPORT (contains-all-of) */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>
#include	<usystem.h>
#include	<dupstr.h>
#include	<six.h>			/* |sichr(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"conallof.h"


/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	CF_BITSET

namespace {
    struct bitsetx : bitset<256> {
	int load(cchar *) noex ;
    } ; /* end struct (bitsetx) */
}

int conallof(cchar *sp,int sl,cchar *tstr) noex {
	int		rs = SR_OK ;
	int		f = true ;
	if (tstr[0] != '\0') {
	    f = false ;
	    if (bitsetx bs ; (rs = bs.load(tstr)) >= 0) {
	        while (sl && *sp) {
		    cint	ch = mkchar(*sp) ;
		    if (bs[ch]) {
			bs.reset(ch) ;
			f = (bs.count() == 0) ;
	                if (f) break ;
		    } /* end if */
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	    } /* end if (bitsetx) */
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (conallof) */

int bitsetx::load(cchar *sp) noex {
	int		rs = SR_OK ;
	try {
	    for (int ch ; (ch = mkchar(*sp++)) > 0 ; ) {
		set(ch) ;
	    } /* end for */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}

#else /* CF_BITSET */

int conallof(cchar *sp,int sl,cchar *tstr) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = true ;
	if (tstr[0] != '\0') {
	    dupstr	sd ;
	    f = false ;
	    if (char *bp{} ; (rs = sd.start(tstr,-1,&bp)) >= 0) {
	        int	bl = rs ;
	        while (sl && *sp) {
		    if (int si ; (si = sichr(bp,bl,sp[0])) >= 0) {
			if (bl-- > 1) {
			    if (si < bl) bp[si] = bp[bl] ;
			}
		        f = (bl == 0) ;
	                if (f) break ;
		    } /* end if (sichr) */
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        rs1 = sd.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (dupstr) */
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (conallof) */

#endif /* CF_BITSET */


