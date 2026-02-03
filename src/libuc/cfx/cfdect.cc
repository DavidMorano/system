/* cfdect SUPPORT */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdckdint>		/* |ckd_mul(3c++)| (global namespace) */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<intsat.h>		/* |intsat(3u)| */
#include	<strn.h>		/* |strnalpha(3uc)| */
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<cfdec.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"cfdect.h"


/* local defines */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */

local int	cfloop(cchar *,int,int *) noex ;


/* local variables */


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
		        res = (- res) ;
		    }
		} /* end if (cfloop) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rp) {
	    *rp = ((rs >= 0) ? res : 0) ;
	}
	if (res < 0) res = 0 ;
	return (rs >= 0) ? res : rs ;
}
/* end subroutine (cfdecti) */


/* local subroutines */

template<typename T> local int convert(cchar *sp,int sl,int mc,T *rp) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) == 1) ylikely {
	    T		mf = 1 ;
	    switch (mc) {
	    case 'Y':
	        mf = 365 * 24 * 60 * 60 ;
	        break ;
	    case 'M':
	        mf = 31 * 24 * 60 * 60 ;
	        break ;
	    case 'W':
	    case 'w':
	        mf = 7 * 24 * 60 * 60 ;
	        break ;
	    case 'D':
	    case 'd':
	        mf = 24 * 60 * 60 ;
	        break ;
	    case 'h':
	        mf = 60 * 60 ;
	        break ;
	    case 'm':
	        mf = 60 ;
	        break ;
	    case 's':
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
	} else {
	    rs = SR_INVALID ;
	} /* end if (non-zero) */
	return rs ;
} /* end subroutine-template (convert) */

local int cfloop(cchar *sp,int sl,int *rp) noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	int		res = 0 ; /* accumulated-result */
	int		inc{} ;
	for (cc *tp ; (rs >= 0) && (tp = strnalpha(sp,sl)) != np ; ) {
	    cint	mch = mkchar(*tp) ;
	    cint	tl = intconv(tp - sp) ;
	    rs = convert(sp,tl,mch,&inc) ;
	    res += inc ;
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	} /* end for */
	if ((rs >= 0) && (sl > 0)) {
	    rs = convert(sp,sl,0,&inc) ;
	    res += inc ;
	}
	*rp = res ;
	return rs ;
} /* end subroutine (cfloop) */


