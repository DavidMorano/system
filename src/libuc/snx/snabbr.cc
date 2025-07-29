/* snabbr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy an abbreviation of a groups of words */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snabbr

	Description:
	Copy an abbreviation of the given string to the destination.

	Synopsis:
	int snabbr(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer pointer
	dl		destination string buffer length
	sp		source string pointer
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sfx.h>			/* |sfnext(3uc)| */
#include	<toxc.h>		/* |chtouc(3uc)| (for GCC b*llsh*t) */
#include	<localmisc.h>

#include	"snx.h"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snabbr(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    cchar	*cp{} ;
	    rs = SR_OK ;
	    if (dl < 0) dl = INT_MAX ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	        if (i < dl) {
	            dp[i++] = chtouc(cp[0]) ; /* <- GCC conversion complaint */
	        } else {
		    rs = SR_OVERFLOW ;
	        }
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp + cl) ;
	        if (rs < 0) break ;
	    } /* end for */
	    dp[i] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (snabbr) */


