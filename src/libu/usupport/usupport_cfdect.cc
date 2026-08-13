/* usupport_cfdect SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert from a decimal string with time codes on the end */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cfdect

	Description:
	This subroutine converts a character string representing a
	time-interval to an integer |int| tpye variable.  The
	time-interval is returned in an integer.  The character
	string representing a time-interval has the form:
		<mum>[<alpha>[<num>[<alpha>[<num>]]]] ...

	Synopsis:
	int cfdect{x}(cchar *sbuf,int slen,{x} *rp) noex

	Arguments:
	{x}		i = |int|, l = |long|, ll = |longlong|
	sbuf		source specification string pointer
	slen		source specification string length
	rp		pointer to integer to receive result

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdckdint>		/* CSTD |ckd_mul(3c++)| */
#include	<bitset>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<intsat.h>		/* LIBU |intsat(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"usupport_cfdect.hh"
	
import ureserve ;			/* |sf{x}(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::bitset ;			/* type */
using libu::cfdec ;			/* subroutine */
using libu::sialpha ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */

constexpr char		chvals[] = "YMDWwdhms" ;

namespace {
    constexpr int	chtablen = (UCHAR_MAX + 1) ;
    struct chvalid {
	bitset<chtablen>	isval ;
	consteval void mkisval() noex {
	    for (cchar *cp = chvals ; *cp ; cp += 1) {
		cint ch = mkchar(*cp) ;
		isval.set(ch) ;
	    } /* end for */
	} ; /* end method (mkisval) */
	consteval chvalid() noex {
	    mkisval() ;
	} ; /* end ctor */
	int operator [] (int ch) const noex {
	    ch &= UCHAR_MAX ;
	    return (isval[ch]) ? SR_OK : SR_INVALID ;
	} ; /* end method (operator) */
    } ; /* end struct (chvalie) */
} /* end namespace */


/* forward references */

local int	cfloop(cchar *,int,int *) noex ;


/* local variables */

constexpr chvalid	tabval ;
const int		oneday		= 24 * 60 * 60 ;
const bool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int cfdecti(cchar *sbuf,int slen,int *rp) noex {
	int		rs = SR_FAULT ;
	int		res = 0 ; /* return-value */
	if (sbuf) ylikely {
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if (int sl ; (sl = sfshrink(sbuf,slen,&sp)) > 0) ylikely {
		bool	fneg = false ;
		res = 0 ;
	        if (sl && (*sp == '-')) {
	            fneg = true ;
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end if (test for negative) */
		if ((rs = cfloop(sp,sl,&res)) >= 0) ylikely {
	            if (fneg) {
		        res = (neg res) ;
		    }
		} /* end if (cfloop) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rp) {
	    *rp = ((rs >= 0) ? res : 0) ;
	}
	if (res < 0) res = 0 ;
	return (rs >= 0) ? res : rs ;
    } /* end subroutine (cfdecti) */
} /* end namespace (libu) */


/* local subroutines */

template<typename T> local int convert(cchar *sp,int sl,int mc,T *rp) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	DPRINTF("ent sl=%d\n",sl) ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) ylikely {
	    T		mf = 1 ;
	    switch (mc) {
	    case 'Y':
	        mf = oneday * 365 ;
	        break ;
	    case 'M':
	        mf = oneday * 31 ;
	        break ;
	    case 'W':
	    case 'w':
	        mf = oneday * 7 ;
	        break ;
	    case 'D':
	    case 'd':
	        mf = oneday * 1 ;
	        break ;
	    case 'h':
	        mf *= 60 ;
	        falldown ;
	    case 'm':
	        mf *= 60 ;
	        break ;
	    case 's':
	    case 0:			/* <- indicates no multiply character */
	        break ;
	    default:
		DPRINTF("invalid code >%c<\n",mc) ;
		rs = SR_NOMSG ;
		break ;
	    } /* end switch */
	    if (rs >= 0) ylikely {
	        if (T v{} ; (rs = cfdec(cp,cl,&v)) >= 0) ylikely {
		    if (T res{} ; (! ckd_mul(&res,v,mf))) ylikely {
		        *rp = res ;
		        rs = intsat(res) ;
		    } else {
			rs = SR_RANGE ;
		    }
	        } /* end if (cfdeci) */
	    } /* end if (ok) */
	} else {
	    rs = SR_INVALID ;
	} /* end if (non-zero) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine-template (convert) */

local int cfloop(cchar *sp,int sl,int *rp) noex {
    	int		rs = SR_OK ;
	int		res = 0 ; /* accumulated-result */
	int		inc{} ;
	DPRINTF("ent sl=%d sp=>%s<\n",sl,sp) ;
	for (int si ; (rs >= 0) && (si = sialpha(sp,sl)) >= 0 ; ) {
	    if (cint mch = mkchar(sp[si]) ; (rs = tabval[mch]) >= 0) {
	        if (si > 0) {
	            rs = convert(sp,si,mch,&inc) ;
	            res += inc ;
	        } /* end if (non-zero positive) */
	    } /* end if (valid) */
	    sl -= (si + 1) ;
	    sp += (si + 1) ;
	} /* end for */
	if ((rs >= 0) && (sl > 0)) {
	    rs = convert(sp,sl,0,&inc) ;
	    res += inc ;
	} /* end if (trailing component) */
	*rp = res ;
	DPRINTF("ret rs=%d v=%d\n",rs,res) ;
	return rs ;
} /* end subroutine (cfloop) */


