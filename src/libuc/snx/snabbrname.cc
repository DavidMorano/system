/* snabbrname SUPPORT */
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
	snabbrname

	Description:
	Copy an abbreviation of the given string to the destination.

	Synopsis:
	int snabbrname[(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer pointer
	dl		destination string buffer length
	sp		source string pointer
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
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
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sfx.h>			/* LIBUC |sfnext(3uc)| */
#include	<toxc.h>		/* LIBUC |chtouc(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"snabbr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

int snabbrname(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) ylikely {
	        cchar	*cp{} ;
	        rs = SR_OK ;
	        if (sl < 0) sl = lenstr(sp) ;
	        for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	            if (i < dl) {
	                dp[i++] = chtouc(cp[0]) ;
	            } else {
		        rs = SR_OVERFLOW ;
	            }
	            sl -= intconv((cp + cl) - sp) ;
	            sp = (cp + cl) ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (non-null) */
	    dp[i] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (snabbrname) */


