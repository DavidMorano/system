/* cfsysx MODULE (primary-module-interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
	cfsysx

	Description:
	This subroutine-template is used to convert digit c-strings
	to binary integers.  This object uses the uderlying (system
	supplied) digit conversion functions to implement its
	interface.  Note that the (so-called) system supplied
	conversion facilities are (clandestinely) supplemented to
	support an interger type of 128-bits (a |longlong| type).
	But the (so-called) system does not generally support integer
	types larger than 128 bits.  So this subroutine-template
	|cfcharx(3uc)| also does not support integer types larger
	than 128-bits.  Also, only digit number bases of omly a
	maximum of 36 is supported.  So any number bases larger
	than 36 require a differnt conversion facility, like for
	example |cfdigx(3uc)|.

	Synopsis:
    	template<typename T> inline int cfsysx(cc *sp,int sl,int b,T *rp) noex

	Arguments:
	sp	digit c-string pointer (to convert to integer)
	sl	digit c-string length  (to convert to integer)
	b	number base
	rp	pointer to result value (of the specified type)

	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<stdintx.h>		/* extended integer types */
#include	<snwcpy.h>
#include	<sfx.h>
#include	<checkbase.h>
#include	<cfutil.hh>		/* namespace |cfx| + |lenstr(3u)| */
#include	<localmisc.h>

export module cfsysx ;

/* max for |int256_t| + sign */
inline constexpr int	cfsysx_maxstack = (256+1) ;

template<typename T> inline int cfsystox(cc *,int,T *) noex {
	return SR_BUGCHECK ;
}
template<> inline int cfsystox(cc *sp,int b,int *rp) noex {
	return uc_strtoi(sp,nullptr,b,rp) ;
}
template<> inline int cfsystox(cc *sp,int b,long *rp) noex {
	return uc_strtol(sp,nullptr,b,rp) ;
}
template<> inline int cfsystox(cc *sp,int b,longlong *rp) noex {
	return uc_strtoll(sp,nullptr,b,rp) ;
}
template<> inline int cfsystox(cc *sp,int b,uint *rp) noex {
	return uc_strtoui(sp,nullptr,b,rp) ;
}
template<> inline int cfsystox(cc *sp,int b,ulong *rp) noex {
	return uc_strtoul(sp,nullptr,b,rp) ;
}
template<> inline int cfsystox(cc *sp,int b,ulonglong *rp) noex {
	return uc_strtoull(sp,nullptr,b,rp) ;
}

export {
    template<typename T> inline int cfsysx(cc *sp,int sl,int b,T *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sp && rp) {
	    cchar	*nsp{} ;
	    rs = SR_DOM ;
	    if (int nsl ; (nsl = cfx::sfdigs(sp,sl,&nsp)) > 0) {
	    	if ((rs = checkbase(nsp,nsl,b)) >= 0) {
		    if (nsp[nsl] != '\0') {
			auto	load = snwcpyshrink ;
			cint	dlen = nsl ;
	                if (nsl <= cfsysx_maxstack) {
	                    char	dbuf[dlen+1] ;
	    	            if ((rs = load(dbuf,dlen,nsp,nsl)) >= 0) {
		                rs = cfsystox(dbuf,b,rp) ;
	                    } /* end if (loading) */
		        } else {
		            char	*dbuf{} ;
		            if ((rs = uc_malloc((dlen+1),&dbuf)) >= 0) {
	    	                if ((rs = load(dbuf,dlen,nsp,nsl)) >= 0) {
		                    rs = cfsystox(dbuf,b,rp) ;
	                        } /* end if (loading) */
			        rs1 = uc_free(dbuf) ;
			        if (rs >= 0) rs = rs1 ;
		            } /* end if (memory-allocation) */
	                } /* end if */
		    } else {
			rs = cfsystox(nsp,b,rp) ;
		    } /* end if */
		} /* end if (checkbase) */
	    } /* end if (non-zero c-string) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfsysx) */
} /* end export */


