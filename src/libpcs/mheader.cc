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

	Notes:
	1. Reminder that header-key names MUST start in column
	zero ('0') with no leading white-space characters (at all).
	Historically, white-space WAS allowed AFTER the header-key
	name and before the colon (':') character.

	See-also:
	hmatch(3pcs)
	mheader(3pcs)
	headkeymat(3mailmsg)
	mailmsgmathdr(3mailmsg)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strncasecmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<six.h>			/* |sichr(3uc)| */
#include	<rmx.h>			/* |rmblk(3uc)| */
#include	<localmisc.h>

#include	"mheader.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/pcsutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import pcsutil ;			/* |hdrmatbasic(3pcs)| */

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

int mheader(cchar *hkeyp,cchar *hbuf) noex {
    	int		rs = SR_FAULT ;
	int		idx = 0 ; /* return-value */
	if (cint hkeyl = getlenstr(hkeyp) ; hkeyl >= 0) {
	    if (cint hlen = getlenstr(hbuf) ; hlen >= 0) {
	        rs = SR_OK ;
	        if (hdrmatbasic(hkeyp,hbuf)) {
	            if (cint si = sichr(hbuf,hlen,':') ; si >= hkeyl) {
		        if (cint kl = rmblk(hbuf,si) ; kl == hkeyl) {
		            if (strncasecmp(hbuf,hkeyp,hkeyl) == 0) {
			        idx = (si + 1) ;
		            }
		        } /* end if (sfnext) */
	            } /* end if (sichr) */
	        } /* end if (hdrmatbasic) */
	    } /* end if (getlenstr) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (mheader) */


