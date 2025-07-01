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

  	Group:
	cfdect

	Description:
	This subroutine converts a character string representing a
	time-interval.  The time-interval is returned in an integer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strn.h>
#include	<sfx.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"cfdect.h"


/* local defines */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */

static int	convert(cchar *,int,int,int *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cfdecti(cchar *sbuf,int slen,int *rp) noex {
	int		rs = SR_FAULT ;
	int		res = 0 ;
	if (sbuf) {
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if (int sl ; (sl = sfshrink(sbuf,slen,&sp)) > 0) {
		bool	f_negative = false ;
	        cchar	*tp ;
		rs = SR_OK ;
		res = 0 ;
	        if (sl && (*sp == '-')) {
	            f_negative = true ;
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        while ((tp = strnbrk(sp,sl,"YMWDwdhms")) != nullptr) {
		    cint	mch = mkchar(*tp) ;
		    cint	tl = intconv(tp - sp) ;
	            rs = convert(sp,tl,mch,&res) ;
	            if (rs < 0) break ;
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	        } /* end while */
	        if ((rs >= 0) && (sl > 0)) {
	    	    rs = convert(sp,sl,0,&res) ;
	        }
	        if (f_negative) {
		    res = (- res) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rp) *rp = ((rs >= 0) ? res : 0) ;
	if (res < 0) res = 0 ;
	return (rs >= 0) ? res : rs ;
}
/* end subroutine (cfdecti) */


/* local subroutines */

static int convert(cchar *sp,int sl,int mc,int *rp) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
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
	    } /* end switch */
	    if (int v{} ; (rs = cfdeci(cp,cl,&v)) >= 0) {
	        *rp += (v * mf) ;
	    }
	} /* end if (non-zero) */
	return rs ;
}
/* end subroutine (convert) */


