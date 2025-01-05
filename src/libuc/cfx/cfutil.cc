/* cfutil SUPPORT */
/* encoding=ISO8859-1 */
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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>		/* <- for |uc_str{xx}(3uc)| */
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<ischarx.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| below */

#include	"cfutil.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace cfx {
    int rmleadzero(cchar *sp,int sl) noex {
	int	nsl = sl ;
	for (int i = 0 ; (i < (sl-1)) && iszero(*sp) ; i += 1) {
	    nsl -= 1 ;
	} /* end for */
	return nsl ;
    }
    int sfdigs(cchar *nsp,int nsl,cchar **rpp) noex {
	int	rl = nsl ;
	cchar	*sp = nsp ;
	if (int sl ; (sl = sfshrink(nsp,nsl,&sp)) > 0) {
	    rl = sl ;
	    if (sl > 1) {
		rl = rmleadzero(sp,sl) ;
		sp += (sl - rl) ;
	    }
	}
	*rpp = sp ;
	return rl ;
    }
} /* end namespace (cfx) */


