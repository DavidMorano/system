/* cfdigx MODULE (primary-module-interface) */
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

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| + |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>		/* extended integer types */
#include	<checkbase.h>
#include	<cfutil.hh>		/* namespace |cfx| + |lenstr(3u)| */
#include	<char.h>		/* |CHAR_TOVAL(3uc)| */
#include	<ischarx.h>		/* |isplusminus(3uc)| */
#include	<localmisc.h>

export module cfdigx ;

import libutil ;

template<stdintx T> struct cfdshelp { /* Convert-From-Digit-SiSigned */
	cint		nb = (CHAR_BIT * szof(T)) ;
	T		*rp = nullptr ;
	T		val = 0 ;	/* value to create */
	T		cutoff ;
	T		tmax = 0 ;	/* type-maximum value */
	T		tmin{} ;	/* type-minimum value */
	int		cutlim ;
	int		sl ;
	int		base ;
	bool		fneg = false ;
	cchar		*sp ;
	cfdshelp(cchar *asp,int asl,int b,T *arp) noex : sp(asp), sl(asl) {
	    const T	one = 1 ;
	    tmin = (one << (nb - 1)) ;
	    tmax = (compl tmax) & (compl tmin) ;
	    base = b ;
	    rp = arp ;
	} ; /* end ctor */
	int getsign() noex {
    	    cint rs = cfx::getsign(sp,sl,&fneg) ;
	    sp += (sl - rs) ;
	    sl = rs ;
	    return rs ;
	} ; /* end if (getsign) */
	void prepare() noex {
	    cutoff = fneg ? tmin : tmax ;
	    cutlim = int(cutoff % base) ;
	    cutoff /= base ;
	    if (fneg) {
		if (cutlim > 0) {
		    cutlim -= base ;
		    cutoff += 1 ;
		}
		cutlim = (- cutlim) ;
	    }
	} ; /* end method (prepare) */
	int getval(int ch) noex {
	    int		rs = CHAR_TOVAL(ch) ;
	    if ((rs == UCHAR_MAX) || (rs >= base)) {
		rs = SR_DOM ;
	    }
	    return rs ;
	} ; /* end method (getval) */
	int proc() noex {
	    int		rs = SR_INVALID ;
	    if (fneg) {
	        while (sl && ((rs = getval(*sp)) >= 0)) {
		    cint	nv = rs ;
		    if ((val < cutoff) || (val == cutoff && nv > cutlim)) {
			rs = SR_RANGE ;
		    } else {
		        val *= base ;
		        val -= nv ;
		    }
		    sp += 1 ;
		    sl -= 1 ;
		    if (rs < 0) break ;
	        } /* end while */
	    } else {
	    	while (sl && ((rs = getval(*sp)) >= 0)) {
		    cint	nv = rs ;
		    if ((val > cutoff) || (val == cutoff && nv > cutlim)) {
			rs = SR_RANGE ;
		    } else {
			val *= base ;
			val += nv ;
		    }
		    sp += 1 ;
		    sl -= 1 ;
		    if (rs < 0) break ;
	        } /* end while */
	    } /* end if (sign) */
	    *rp = (rs >= 0) ? val : 0 ;
	    return rs ;
	} ; /* end method (proc) */
	operator int () noex {
	    int		rs ;
	    if ((rs = getsign()) >= 0) {
		prepare() ;
		rs = proc() ;
	    }
	    return rs ;
	} ;
} ; /* end struct (cfdshelp -- Convert-From-Digit-SiSigned) */

template<stdintx T> struct cfduhelp { /* Convert-From-Digit-UnSigned */
	T		*rp = nullptr ;
	T		val = 0 ;	/* value to create */
	T		cutoff ;
	T		tmax = 0 ;	/* type-maximum value */
	int		cutlim ;
	int		sl ;
	int		base ;
	bool		fneg = false ;
	cchar		*sp ;
	cfduhelp(cchar *asp,int asl,int b,T *arp) noex : sp(asp), sl(asl) {
	    tmax = (compl T{0}) ;
	    base = b ;
	    rp = arp ;
	} ; /* end ctor */
	int getsign() noex {
    	    cint rs = cfx::getsign(sp,sl,&fneg) ;
	    sp += (sl - rs) ;
	    sl = rs ;
	    return rs ;
	} ; /* end if (getsign) */
	void prepare() noex {
	    cuint	ub = uint(base) ;
	    cutoff = (tmax / ub) ;
	    cutlim = int(tmax % ub) ;
	} ; /* end method (prepare) */
	int getval(int ch) noex {
	    int		rs = CHAR_TOVAL(ch) ;
	    if ((rs == UCHAR_MAX) || (rs >= base)) {
		rs = SR_DOM ;
	    }
	    return rs ;
	} ; /* end method (getval) */
	int proc() noex {
	    int		rs = SR_INVALID ;
	    {
	        while (sl && ((rs = getval(*sp)) >= 0)) {
		    cint	nv = rs ;
		    if ((val > cutoff) || (val == cutoff && nv > cutlim)) {
			rs = SR_RANGE ;
		    } else {
		        val *= base ;
		        val += nv ;
		    }
		    sp += 1 ;
		    sl -= 1 ;
		    if (rs < 0) break ;
	        } /* end while */
	    } /* end block */
	    if (rs >= 0) {
		*rp = (fneg) ? (- val) : val ;
	    } else {
		*rp = 0 ;
	    }
	    return rs ;
	} ; /* end method (proc) */
	operator int () noex {
	    int		rs ;
	    if ((rs = getsign()) >= 0) {
		prepare() ;
		rs = proc() ;
	    }
	    return rs ;
	} ;
} ; /* end struct (cfduhelp -- Convert-From-Digit-UnSigned) */

export {
    template<typename ST> inline int cfdigsx(cc *sp,int sl,int b,ST *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	   cfdshelp cfo(sp,sl,b,rp) ;
	   rs = cfo ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfdigsx) */
    template<typename UT> inline int cfdigux(cc *sp,int sl,int b,UT *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	   cfduhelp cfo(sp,sl,b,rp) ;
	   rs = cfo ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfdigux) */
} /* end export */


