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
	int cfdecti(cchar *sbuf,int slen,int *rp) noex

	Arguments:
	sbuf		source specification string pointer
	slen		source specification string length
	rp		pointer to integer to receive result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
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
local int	convert(cchar *,int,int,int *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cfdecti(cchar *sbuf,int slen,int *rp) noex {
	int		rs = SR_FAULT ;
	int		res = 0 ;
	if (sbuf) ylikely {
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if (int sl ; (sl = sfshrink(sbuf,slen,&sp)) > 0) ylikely {
		bool	f_negative = false ;
		res = 0 ;
	        if (sl && (*sp == '-')) {
	            f_negative = true ;
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end if (test for negative) */
		if ((rs = cfloop(sp,sl,&res)) >= 0) ylikely {
	            if (f_negative) {
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

local int convert(cchar *sp,int sl,int mc,int *rp) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) ylikely {
	    int		mf = 1 ;
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
	        if (int v{} ; (rs = cfdeci(cp,cl,&v)) >= 0) {
	            cint res = (v * mf) ;
		    *rp = res ;
		    rs = intsat(res) ;
	        } /* end if (cfdeci) */
	    } /* end if (ok) */
	} /* end if (non-zero) */
	return rs ;
} /* end subroutine (convert) */


