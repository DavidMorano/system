/* cfutil SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written adapted from assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfutil

	Description:
	Subroutines to convert decimal strings to binary floating
	values.

	Synopsis:
	int cfutil(cchar *sp,int sl,double *rp) noex

	Arguments:
	sp		source string
	sl		source string length
	rp		pointer to hold result

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<char.h>
#include	<ischarx.h>		/* |iszero(3uc)| */
#include	<localmisc.h>

#include	"cfutil.hh"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace cfx {
    int rmleadzero(cchar *sp,int sl) noex {		/* used internally */
	if ((sl > 1) && iszero(*sp++)) {
	    sl -= 1 ;
	}
	return sl ;
    } /* end subroutine (rmleadzero) */
    int sfdigs(cchar *nsp,int nsl,cchar **rpp) noex {
	int	rl = nsl ;
	cchar	*sp = nsp ;
	if (int sl ; (sl = sfshrink(nsp,nsl,&sp)) > 0) {
	    rl = sl ;
	    if (sl > 1) {
		rl = rmleadzero(sp,sl) ;
		sp += (sl - rl) ;
	    }
	} /* end if (sfshrink) */
	*rpp = sp ;
	return rl ;
    } /* end subroutine (cfdigs) */
    int sfchars(cchar *nsp,int nsl,cchar **rpp) noex {
	int	rl = 0 ;
	cchar	*sp = nsp ;
	if (int sl ; (sl = sfshrink(nsp,nsl,&sp)) > 0) {
	    cauto islead = [] (char ch) noex {
		return ((ch == ' ') || (ch == '+') || (ch == '0')) ;
	    } ;
	    while ((sl > 0) && islead(*sp)) {
		sp += 1 ;
		sl -= 1 ;
	    }
	    rl = sl ;
	} /* end if (shshrink) */
	*rpp = sp ;
	return rl ;
    } /* end subroutine (cfchars) */
} /* end namespace (cfx) */

namespace cfx {
    int getsign(cchar *sp,int sl,bool *fnegp) noex {
	int		rs = SR_FAULT ;
	if (sp && fnegp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if ((sl > 0) && isplusminus(*sp)) {
	        *fnegp = (*sp == '-') ;
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (sl > 1) {
		if (iszero(*sp)) {
		    sp += 1 ;
		    sl -= 1 ;
		}
	    } else if (sl == 0) {
		rs = SR_INVALID ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
    } ; /* end if (getsign) */
} /* end namespace (cfx) */


