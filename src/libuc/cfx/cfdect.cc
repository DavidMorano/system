/* cfdect SUPPORT (Convert-From-Decimal-Time) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert from a decimal string with time codes on the end */
/* version %I% last-modified %G% */


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
#include	<intsat.h>		/* LIBU |intsat(3u)| */
#include	<six.h>			/* LIBUC |sialpha(3uc)| */
#include	<sfx.h>			/* LIBUC |sfshrink(3uc)| */
#include	<cfdec.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cfdect.h"


/* local defines */


/* imported namespaces */

using std::bitset ;


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */

constexpr char          chvals[] = "YMDWwdhms" ;

namespace {
    constexpr int       chtablen = (UCHAR_MAX + 1) ;
    struct chvalid {
        bitset<chtablen>        isval ;
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

constexpr chvalid       tabval ;
constexpr int		oneday		= 24 * 60 * 60 ;


/* exported variables */


/* exported subroutines */

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


/* local subroutines */

template<typename T> local int convert(cchar *sp,int sl,int mc,T *rp) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
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
	        mf = 60 * 60 ;
	        break ;
	    case 'm':
	        mf = 60 ;
	        break ;
	    case 's':
	    case 0:			/* <- indicates no multiply character */
	        break ;
	    default:
		rs = SR_NOMSG ;
		break ;
	    } /* end switch */
	    if (rs >= 0) {
	        if (T v{} ; (rs = cfdec(cp,cl,&v)) >= 0) {
		    if (T res{} ; (! ckd_mul(&res,v,mf))) ylikely {
		        *rp = res ;
		        rs = intsat(res) ;
		    } else {
			rs = SR_RANGE ;
		    }
	        } /* end if (cfdeci) */
	    } /* end if (ok) */
	} /* end if (non-zero) */
	return rs ;
} /* end subroutine-template (convert) */

local int cfloop(cchar *sp,int sl,int *rp) noex {
    	int		rs = SR_OK ;
	int		res = 0 ; /* accumulated-result */
	int		inc{} ;
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
	} /* end if (remainder) */
	*rp = res ;
	return rs ;
} /* end subroutine (cfloop) */


