/* cfalphax HEADER */
/* lang=C++20 */

/* conversion of power-base-two c-string to their binary integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-05-14, David A­D­ Morano
	Conver numeric digit c-strings into integers.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code converts a c-string of digits (of a power-of-two
	base) into the normal integer types: these being |int|,
	|long|, and |longlong| and their associated unsigned
	variations.


		-- David A.D. Morano, 2023-10-10

	Notes:
	1. This code is limited to a maximum base of 64.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<climits>		/* for |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<char.h>
#include	<ischarx.h>


template<typename T>
struct cfpowshelp {
	cint		nb = (CHAR_BIT * sizeof(T)) ;
	T		*rp = nullptr ;
	T		val = 0 ;	/* value to create */
	T		cutoff{} ;
	T		tmax = 0 ;
	T		tmin{} ;
	int		cutlim ;
	int		sl ;
	int		base ;
	bool		fneg = false ;
	cchar		*sp ;
	cfpowshelp(cchar *asp,int asl,int b,T *arp) noex : sp(asp), sl(asl) {
	    const T	one = 1 ;
	    tmin = (one << (nb - 1)) ;
	    tmax = (~ tmax) & (~ tmin) ;
	    base = b ;
	    rp = arp ;
	} ; /* end ctor */
	void prepare() noex {
	    cutoff = fneg ? tmin : tmax ;
	    cutlim = (cutoff % base) ;
	    cutoff /= base ;
	    if (fneg) {
		if (cutlim > 0) {
		    cutlim -= base ;
		    cutoff += 1 ;
		}
		cutlim = (-cutlim) ;
	    }
	} ; /* end method (prepare) */
	int getval(int ch) noex {
	    int		rs = SR_INVALID ;
	    if ((ch >= 'A') && (ch <= 'Z')) {
		rs = (ch - 'A') ;
	    } else if ((ch >= 'a') && (ch <= 'z')) {
		rs = (ch - 'a') ;
	    }
	    if (rs >= base) {
		rs = SR_DOM ;
	    }
	    return rs ;
	} ; /* end method (getval) */
	int getsign() noex {
	    int		rs = SR_FAULT ;
	    if (rp) {
	        if (sl < 0) sl = strlen(sp) ;
	        while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        if ((sl > 0) && isplusminus(*sp)) {
	            fneg = (*sp == '-') ;
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        if (sl == 0) rs = SR_INVALID ;
	    } /* end if (non-null) */
	    return rs ;
	} ; /* end if (getsign) */
	int proc() noex {
	    int		rs = SR_INVALID ;
	    while (sl && *sp && ((rs = getval(*sp)) >= 0)) {
		cint	nv = rs ;
		if (fneg) {
		    if ((val < cutoff) || (val == cutoff && nv > cutlim)) {
			rs = SR_RANGE ;
		    } else {
		        val *= base ;
		        val -= nv ;
		    }
		} else {
		    if ((val > cutoff) || (val == cutoff && nv > cutlim)) {
			rs = SR_RANGE ;
		    } else {
			val *= base ;
			val += nv ;
		    }
		}
		if (rs < 0) break ;
	    } /* end while */
	    *rp = (rs >= 0) ? val : 0 ;
	    return rs ;
	} ; /* end method (proc) */
} ; /* end struct (cfpowshelp) */

template<typename T>
int cfalphax(cchar *sp,int sl,int b,T *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    cfpowshelp	cfo(sp,sl,b,rp) ;
	    if ((rs = cfo.getsign()) >= 0) {
		cfo.prepare() ;
		rs = cfo.proc() ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (cfalphax) */


