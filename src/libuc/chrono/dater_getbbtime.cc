/* dater_getbbtime SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* extension to DATER object to parse BBNEWS time-stamps */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	We collect the code that accesses the user currency file
	pretty much into one place. The functions handled by this
	module were previously scattered around in the past!

	= 1998-11-13, David A­D­ Morano
	This is enhanced from the older version of the same (that
	I wrote back in the early 90s).

*/

/* Copyright © 1994,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dater_getbbtime

	Description:
	This subroutine parses BBNEWS time-stamps into DATER objects.

	Synopsis:
	int dater_getbbtime(dater *op,cchar *sp,int sl,time_t *tp) noex

	Arguments:
	op	DATER object pointer
	sp	source c-string pointer
	sl	source c-string length
	tp	result time

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sfx.h>
#include	<cfdec.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| + |TIMEBUFLEN| */

#include	"dater.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dater_getbbtime(dater *op,cchar *sp,int sl,time_t *tp) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp,tp)) >= 0) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	        if ((cl == 1) && (cp[0] == '0')) {
	            *tp = 0 ;
	        } else if ((rs = dater_setstrdig(op,cp,cl)) >= 0) {
	            rs = dater_gettime(op,tp) ;
	        } else if (rs == SR_INVALID) {
	            if (ulong ulw ; (rs = cfdecul(cp,cl,&ulw)) >= 0) {
	                *tp = time_t(ulw) ;
	            }
	        } /* end if */
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getbbtime) */


