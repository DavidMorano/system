/* mheader SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* determine if a given c-string contains a mail-message header key */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mheader

	Description:
	Does a proper substring of given c-string to test match the
	given header-key c-string?  Return 0 if there is no match,
	else we return the character position of the header-value
	string (one character beyond the colon character).  The
	match is case independent.

	Synopsis:
	int mheader(cchar *hkeyp,cchar *ts) noex

	Arguments:
	hkeyp		header-key c-string to match against
	ts		c-string to test for given header-key

	Returnrs:
	>0		got a match (and the index of the header-value)
	==0		no match
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strncasecmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<six.h>			/* |sichr(3uc)| */
#include	<sfx.h>			/* |sfnext(3uc)| */
#include	<localmisc.h>

#include	"mheader.h"

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

int mheader(cchar *hkeyp,cchar *ts) noex {
    	int		rs = SR_FAULT ;
	int		f = false ;
	if (int hkeyl ; ts && (hkeyl = getlenstr(hkeyp)) >= 0) {
	    rs = SR_OK ;
	    if (int si ; (si = sichr(ts,':')) >= hkeyl) {
		cchar *cp ;
		if (int cl ; (cl = sfnext(ts,si,&cp)) == hkeyl) {
		    if (strncasecmp(hkeyp,cp,cl) == 0) {
			f = (si + 1) ;
		    }
		} /* end if (sfnext) */
	    } /* end if (sichr) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mheader) */


