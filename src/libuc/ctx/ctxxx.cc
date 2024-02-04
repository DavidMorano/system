/* ctxxx SUPPORT */
/* lang=C++20 */

/* subroutines to convert an integer to a decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written having been adapated (from
	memory) from something I wrote back in the early 1980s (for
	embedded work).  I had to write every ... last  ... thing
	myself back in the old days.

	= 2020-04-23, David A-D- Morano
	I updated the |ffbsi| subroutine below to use the new C++20
	find-first-bit-set intrinsic (often a single machine
	instruction).

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
	conversions routinely available. To really move (execute)
	quickly through the division-related aspects of the require
	algorithm, one would have to use assembly language where
	both the quotient and the reminder of a division are produced
	simultaneously (since each are needed to continue). This,
	of course, assumes that the underlying machine architecture
	has such instructions. But short of assembly (and and the
	required machine instructions) this present implemtnation
	is adequate.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<bit>
#include	<usystem.h>		/* <- memory-allocation */
#include	<stdintx.h>
#include	<ucvariables.hh>
#include	<varnames.hh>
#include	<sncpyx.h>

#include	"ctxxx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */

static inline constexpr int ffbsi(int b) noex {
	cuint	n = uint(b) ;
	return std::countr_zero(n) ;	/* <- first bit set */
}


/* local variables */

static const int	maxbase = strlen(varname.digtab) ;
constexpr int		maxstack = (1024+1) ;


/* local subroutine-templates */

template<typename UT>
static int ctxxxx(char *dbuf,int dlen,int b,UT &v) noex {
	cuint		ub(b) ;
	char		*rp = (dbuf + dlen) ;
	*rp = '\0' ;
	if (v != 0) {
	    int		di ;
	    if constexpr (sizeof(UT) > sizeof(ulong)) {
	        const UT	vmask(~ULONG_MAX) ;
	        UT		nv ;
	        while ((v & vmask) != 0UL) {
	            nv = v / ub ;
		    di = int(v - (nv * ub)) ;
		    *--rp = varname.digtab[di] ;
		    v = nv ;
	        } /* end while (slower) */
	        {
		    ulong	lv = (ulong) v ;
		    ulong	nv ;
		    while (lv != 0) {
	                nv = lv / ub ;
			di = int(lv - (nv * ub)) ;
	                *--rp = varname.digtab[di] ;
	                lv = nv ;
		    } /* end while */
		    v = lv ;
	        } /* end block (faster) */
	    } else {
		UT		nv ;
	        while (v != 0) {
	            nv = v / ub ;
		    di = int(v - (nv * ub)) ;
		    *--rp = varname.digtab[di] ;
	            v = nv ;
	        } /* end while (regular) */
	    } /* end if (constexpr) */
	} else {
	    *--rp = '0' ;
	}
	return (dbuf + dlen - rp) ;
}
/* end subroutine (ctxxxx) */

template<typename UT,typename T>
int sctxxxx(char *dp,int dl,int b,UT,T &v) noex {
	UT		ulv = (UT) v ;
	cint		n = sizeof(T) ;
	int		rs = SR_FAULT ;
	if (v < 0) ulv = (- ulv) ;
	if (dp) {
	    cint	t = ffbsi(n) ;
	    rs = SR_NOTSUP ;
	    if ((b >= 2) && (b <= maxbase)) {
	        cint	dlen = digbufsize.bufsize[t][b] ;
		int	len ;
		if (dlen > maxstack) {
		    int		rs1 ;
		    char	*dbuf ;
		    if ((rs = uc_malloc((dlen+1),&dbuf)) >= 0) {
			{
		            len = ctxxxx(dbuf,dlen,b,ulv) ;
		            if (v < 0) dbuf[dlen-(++len)] = '-' ;
		            rs = sncpy1(dp,dl,(dbuf + dlen - len)) ;
			}
			rs1 = uc_free(dbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (memory-allocation-deallocation) */
		} else {
		    char	dbuf[dlen+1] ;
		    len = ctxxxx(dbuf,dlen,b,ulv) ;
		    if (v < 0) dbuf[dlen-(++len)] = '-' ;
		    rs = sncpy1(dp,dl,(dbuf + dlen - len)) ;
		} /* end block */
	   } /* end if (supported base) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (sctxxxx) */

template<typename UT>
int uctxxxx(char *dp,int dl,int b,UT &uv) noex {
	cint		n = sizeof(UT) ;
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	t = ffbsi(n) ;
	    rs = SR_NOTSUP ;
	    if ((b >= 2) && (b <= maxbase)) {
	        cint	dlen = digbufsize.bufsize[t][b] ;
		int	len ;
		{
		    char	dbuf[dlen+1] ;
		    len = ctxxxx(dbuf,dlen,b,uv) ;
		    rs = sncpy1(dp,dl,(dbuf + dlen - len)) ;
		} /* end block */
	    } /* end if (base supported) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (uctxxxx) */


/* exported variables */


/* exported subroutines */

int ctxxxi(char *dp,int dl,int b,int v) noex {
	const uint		uv = 0 ;
	return sctxxxx(dp,dl,b,uv,v) ;
}

int ctxxxl(char *dp,int dl,int b,long v) noex {
	const ulong		uv = 0 ;
	return sctxxxx(dp,dl,b,uv,v) ;
}

int ctxxxll(char *dp,int dl,int b,longlong v) noex {
	const ulonglong		uv = 0 ;
	return sctxxxx(dp,dl,b,uv,v) ;
}

int ctxxxui(char *dp,int dl,int b,uint v) noex {
	return uctxxxx(dp,dl,b,v) ;
}

int ctxxxul(char *dp,int dl,int b,ulong v) noex {
	return uctxxxx(dp,dl,b,v) ;
}

int ctxxxull(char *dp,int dl,int b,ulonglong v) noex {
	return uctxxxx(dp,dl,b,v) ;
}


