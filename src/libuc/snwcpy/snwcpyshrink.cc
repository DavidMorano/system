/* snwcpyshrink SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy the shrunken part of a source string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyshrink

	Description:
	This subroutine constructs a destination string from a
	shrunken source string.

	Synopsis:
	int snwcpyshrink(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination buffer pointer
	dl		destination buffer length
	sp		source string pointer
	sl		source string length

	Returns:
	-		length of found string (in destination buffer)

	Notes:
	Q. Should we bother with ignoring all data after an NL character?
	A. Good question.  I do not know the answer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<localmisc.h>

#include	"snwcpy.h"		/* also for |snwcpy(3uc)| */


/* local defines */

#ifndef	CF_ONEPASS
#define	CF_ONEPASS	1		/* try one-pass over the string */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpyshrink(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dbuf && sp) {
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,CH_NL)) != nullptr) {
	        sl = intconv(tp - sp) ;
	    }
	    if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	        rs = snwcpy(dbuf,dlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snwcpyshrink) */


