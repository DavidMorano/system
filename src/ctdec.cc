/* ctdec SUPPORT */
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
	ctdecxx

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a decimal string that is placed into the caller supplied
	buffer (of specified length).

	Synopsis:
	int ctdecXX(char *rbuf,int rlen,XX v) noex

	Arguments:
	rbuf		caller supplied buffer
	rlen		caller supplied buffer length
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion

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
#include	<bit>			/* <- for |countr_zero(3c++)| */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<uvariables.hh>
#include	<sncpyx.h>

#include	"ctdec.h"


/* local defines */

using std::countr_zero ;		/* subroutine-template */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline constexpr int ffbsi(int b) noex {
	cuint	n = uint(b) ;
	return countr_zero(n) ;	/* <- first bit set */
}


/* local variables */

static constexpr int	base = 10 ;


/* local subroutine-templates */

template<typename UT>
static int ctdecx(char *dbuf,int dlen,UT v) noex {
	char		*rp = (dbuf + dlen) ;
	*rp = '\0' ;
	if (v != 0) {
	    constexpr uint	ub = uint(base) ;
	    if_constexpr (sizeof(UT) > sizeof(ulong)) {
	        const UT	vmask(~ULONG_MAX) ;
	        UT		nv ;
	        while ((v & vmask) != 0UL) {
	            nv = v / ub ;
	            *--rp = (char) ((v - (nv * ub)) + '0') ;
	            v = nv ;
	        } /* end while (slower) */
	        {
		    ulong	lv = (ulong) v ;
		    ulong	nv ;
		    while (lv != 0) {
	                nv = lv / ub ;
	                *--rp = (char) ((lv - (nv * ub)) + '0') ;
	                lv = nv ;
		    } /* end while */
		    v = lv ;
	        } /* end block (faster) */
	    } else {
		UT		nv ;
	        while (v != 0) {
	            nv = v / ub ;
	            *--rp = (char) ((v - (nv * ub)) + '0') ;
	            v = nv ;
	        } /* end while (regular) */
	    } /* end if (constexpr) */
	} else {
	    *--rp = '0' ;
	}
	return (dbuf + dlen - rp) ;
}
/* end subroutine (ctdecx) */

template<typename UT,typename ST>
int sctdecx(char *dp,int dl,const ST &v) noex {
	UT		ulv = (UT) v ;
	cint		n = sizeof(ST) ;
	int		rs = SR_FAULT ;
	if (v < 0) ulv = (- ulv) ;
	if (dp) {
	    cint	t = ffbsi(n) ;
	    {
	        cint	dlen = digbufsize.bufsize[t][base] ;
		{
		    char	dbuf[dlen+1] ;
		    int		len ;
		    len = ctdecx(dbuf,dlen,ulv) ;
		    if (v < 0) dbuf[dlen-(++len)] = '-' ;
		    rs = sncpy1(dp,dl,(dbuf + dlen - len)) ;
		} /* end block */
	   } /* end block */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (sctdecx) */

template<typename UT>
int uctdecx(char *dp,int dl,const UT &uv) noex {
	cint		n = sizeof(UT) ;
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	t = ffbsi(n) ;
	    {
	        cint	dlen = digbufsize.bufsize[t][base] ;
		{
		    char	dbuf[dlen+1] ;
		    int		len ;
		    len = ctdecx(dbuf,dlen,uv) ;
		    rs = sncpy1(dp,dl,(dbuf + dlen - len)) ;
		} /* end block */
	    } /* end block */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (uctdecx) */


/* exported variables */


/* exported subroutines */

int ctdeci(char *dp,int dl,int v) noex {
	return sctdecx<uint>(dp,dl,v) ;
}

int ctdecl(char *dp,int dl,long v) noex {
	return sctdecx<ulong>(dp,dl,v) ;
}

int ctdecll(char *dp,int dl,longlong v) noex {
	return sctdecx<ulonglong>(dp,dl,v) ;
}

int ctdecui(char *dp,int dl,uint v) noex {
	return uctdecx(dp,dl,v) ;
}

int ctdecul(char *dp,int dl,ulong v) noex {
	return uctdecx(dp,dl,v) ;
}

int ctdecull(char *dp,int dl,ulonglong v) noex {
	return uctdecx(dp,dl,v) ;
}


