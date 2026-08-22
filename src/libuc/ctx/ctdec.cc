/* ctdec SUPPORT */
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
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ctdec.h"

#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/digtab.ccm"

import uconstants ;			/* |digbufsize(3u)| */
import typecodes ;			/* |typecode(3u)| */
import digtab ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int           div100 = 100 ;


/* forward references */


/* local variables */

constexpr int		base = 10 ;

local inline char getdig_lo(int) noex ;
local inline char getdig_hi(int) noex ;


/* local subroutine-templates */

template<typename UT>
local constexpr int ctdecx(char *dbuf,int dlen,UT v) noex {
	constexpr uint	udiv100 = uint(div100) ;
	char		*rp = (dbuf + dlen) ;
	int		rl = 0 ; /* return-value */
	*rp = '\0' ;
	if (v != 0) ylikely {
	    int di ;
	    if_constexpr (szof(UT) > szof(ulong)) {
                const UT vmask = (compl UT(ULONG_MAX)) ;
	        for (UT nv ; v & vmask ; v = nv) {
	            nv = v / udiv100 ;
		    di = int(v % udiv100) ;
		    *--rp = getdig_lo(di) ;
		    *--rp = getdig_hi(di) ;
	        } /* end for (slower) */
	        for (ulong nlv, lv = ulong(v) ; lv ; lv = nlv) {
		    nlv = lv / udiv100 ;
		    di = int(lv % udiv100) ;
		    *--rp = getdig_lo(di) ;
		    *--rp = getdig_hi(di) ;
		} /* end for (faster) */
	    } else {
	        for (UT nv ; v ; v = nv) {
		    nv = v / udiv100 ;
		    di = int(v % udiv100) ;
		    *--rp = getdig_lo(di) ;
		    *--rp = getdig_hi(di) ;
	        } /* end for (regular) */
	    } /* end if (constexpr) */
	    if ((rl = intconv(dbuf + dlen - rp)) > 1) {
                while ((rl > 1) && (*rp == '0')) {
                    rp += 1 ;
                    rl -= 1 ;
                } /* end while */
            } /* end if */
	} else {
	    rl = 1 ;
	    *--rp = '0' ;
	} /* end if */
	return rl ;
} /* end subroutine-template (ctdecx) */

template<typename UT,typename ST>
local int ctdecsx(char *dp,int dl,const ST &v) noex {
	UT		uv = (UT) v ;
	cint		tc = typecode<ST> ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (v < 0) uv = (neg uv) ;
	if (dp) ylikely {
	    {
	        cint	dlen = digbufsize.bufsize[tc][base] ;
		int	len ;
		{
		    char dbuf[dlen+1] ;
		    len = ctdecx(dbuf,dlen,uv) ;
		    if (v < 0) dbuf[dlen-(++len)] = '-' ;
		    rs = sncpy(dp,dl,(dbuf + dlen - len)) ;
		    rl = rs ;
		} /* end block */
	   } /* end block */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine-template (ctdecsx) */

template<typename UT>
local int ctdecux(char *dp,int dl,const UT &uv) noex {
	cint		tc = typecode<UT> ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dp) ylikely {
	    {
	        cint	dlen = digbufsize.bufsize[tc][base] ;
		int	len ;
		{
		    char dbuf[dlen+1] ;
		    len = ctdecx(dbuf,dlen,uv) ;
		    rs = sncpy(dp,dl,(dbuf + dlen - len)) ;
		    rl = rs ;
		} /* end block */
	    } /* end block */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine-template (ctdecux) */


/* exported variables */


/* exported subroutines */

int ctdeci(char *dp,int dl,int v)		noex {
	return ctdecsx<uint>(dp,dl,v) ;
} /* end */

int ctdecl(char *dp,int dl,long v)		noex {
	return ctdecsx<ulong>(dp,dl,v) ;
} /* end */

int ctdecll(char *dp,int dl,longlong v)		noex {
	return ctdecsx<ulonglong>(dp,dl,v) ;
} /* end */

int ctdecui(char *dp,int dl,uint uv)		noex {
	return ctdecux(dp,dl,uv) ;
} /* end */

int ctdecul(char *dp,int dl,ulong uv)		noex {
	return ctdecux(dp,dl,uv) ;
} /* end */

int ctdecull(char *dp,int dl,ulonglong uv)	noex {
	return ctdecux(dp,dl,uv) ;
} /* end */


/* local subroutines */

local char getdig_lo(int r) noex {
        return digtab.dig100[r][0] ;
} /* end */

local char getdig_hi(int r) noex {
        return digtab.dig100[r][1] ;
} /* end */


