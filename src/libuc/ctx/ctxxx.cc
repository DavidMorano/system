/* ctxxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines to convert an integer to a decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written having been adapted (from
	memory) from something I wrote back in the early 1980s (for
	embedded work).  I had to write every ... last  ... thing
	myself back in the old days.

	= 2020-04-23, David A-D- Morano
	I updated the code to use the |typecode(3u)| pseudo-intrinsic
	(a templated variable) instead of |ffbs(3u)|.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctxxxxx

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a decimal string that is placed into the caller supplied
	buffer (of specified length).

	Synopsis:
	int ctxxxXX(char *rbuf,int rlen,int b,XX v) noex

	Arguments:
	XX		type is one of: i, l, ll, ui, ul, ull
	rbuf		caller supplied buffer
	rlen		caller supplied buffer length
	b		number base
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion (system-return)

	Notes:
	As it stands now, these subroutines do not perform any funny
	business in trying to make this process faster! These
	subroutines are, therefore, probably the slowest such
	conversions routinely available.  To really move (execute)
	quickly through the division-related aspects of the require
	algorithm, one would have to use assembly language where
	both the quotient and the reminder of a division are produced
	simultaneously (since each are needed to continue).  This,
	of course, assumes that the underlying machine architecture
	has such instructions.  But short of assembly (and and the
	required machine instructions) this present implemtnation
	is adequate.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |ULONG_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bit>			/* C++STD |countr_zero(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ctxxx.h"

#pragma		GCC dependency		"mod/digtab.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import digtab ;				/* |getdig(3u)| + |maxbase(3u)| */
import uconstants ;			/* |digbufsize(3u)| */
import typecodes ;			/* |typecode(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		maxstack = (256 + 1) ;	/* |int256_t| in binary */


/* local subroutine-templates */

template<typename UT>
local constexpr int ctxxxx(char *dbuf,int dlen,int b,UT v) noex {
	cuint		ub(b) ;
	int		rl = 0 ; /* return-value */
	char		*rp = (dbuf + dlen) ;
	*rp = '\0' ;
	if (v != 0) {
	    int di ;
	    if_constexpr (szof(UT) > szof(ulong)) {
                const UT vmask = (compl UT(ULONG_MAX)) ;
	        for (UT nv ; v & vmask ; v = nv) {
	            nv = v / ub ;
		    di = int(v % ub) ;
		    *--rp = getdig(di) ;
	        } /* end for (slower) */
	        for (ulong nlv, lv = ulong(v) ; lv ; lv = nlv) {
		    nlv = lv / ub ;
		    di = int(lv % ub) ;
	            *--rp = getdig(di) ;
		} /* end for (faster) */
	    } else {
	        for (UT nv ; v ; v = nv) {
	            nv = v / ub ;
		    di = int(v % ub) ;
		    *--rp = getdig(di) ;
	        } /* end for (regular) */
	    } /* end if_constexpr (size-of-operand) */
	    rl = intconv(dbuf + dlen - rp) ;
	} else {
	    rl = 1 ;
	    *--rp = '0' ;
	}
	return rl ;
} /* end subroutine (ctxxxx) */

template<typename UT,typename ST>
local int sctxxxx(char *dp,int dl,int b,const ST &v) noex {
	UT		uv = (UT) v ;
	cint		tc = typecode<ST> ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	if (v < 0) uv = (neg uv) ;
	if (dp) {
	    rs = SR_NOTSUP ;
	    if ((b >= 2) && (b <= digtab.maxbase)) {
	        cint	dlen = digbufsize.bufsize[tc][b] ;
		int	len ;
		if (dlen > maxstack) {
		    if (char *dbuf ; (rs = lm_mall((dlen+1),&dbuf)) >= 0) {
			{
		            len = ctxxxx(dbuf,dlen,b,uv) ;
		            if (v < 0) dbuf[dlen - (++len)] = '-' ;
		            rs = sncpy(dp,dl,(dbuf + dlen - len)) ;
			    rl = rs ;
			}
			rs1 = lm_free(dbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (memory-acquire-deallocation) */
		} else {
		    char dbuf[dlen+1] ;
		    len = ctxxxx(dbuf,dlen,b,uv) ;
		    if (v < 0) dbuf[dlen - (++len)] = '-' ;
		    rs = sncpy(dp,dl,(dbuf + dlen - len)) ;
		    rl = rs ;
		} /* end block */
	   } /* end if (supported base) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine-template (sctxxxx) */

template<typename UT>
local int uctxxxx(char *dp,int dl,int b,const UT &uv) noex {
	cint		tc = typecode<UT> ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dp) {
	    rs = SR_NOTSUP ;
	    if ((b >= 2) && (b <= digtab.maxbase)) {
	        cint	dlen = digbufsize.bufsize[tc][b] ;
		int	len ;
		{
		    char dbuf[dlen+1] ;
		    len = ctxxxx(dbuf,dlen,b,uv) ;
		    rs = sncpy(dp,dl,(dbuf + dlen - len)) ;
		    rl = rs ;
		} /* end block */
	    } /* end if (base supported) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine-template (uctxxxx) */


/* exported variables */


/* exported subroutines */

int ctxxxi(char *dp,int dl,int b,int v)			noex {
	return sctxxxx<uint>(dp,dl,b,v) ;
} /* end */

int ctxxxl(char *dp,int dl,int b,long v)		noex {
	return sctxxxx<ulong>(dp,dl,b,v) ;
} /* end */

int ctxxxll(char *dp,int dl,int b,longlong v)		noex {
	return sctxxxx<ulonglong>(dp,dl,b,v) ;
} /* end */

int ctxxxui(char *dp,int dl,int b,uint uv)		noex {
	return uctxxxx(dp,dl,b,uv) ;
} /* end */

int ctxxxul(char *dp,int dl,int b,ulong uv)		noex {
	return uctxxxx(dp,dl,b,uv) ;
} /* end */

int ctxxxull(char *dp,int dl,int b,ulonglong uv)	noex {
	return uctxxxx(dp,dl,b,uv) ;
} /* end */


