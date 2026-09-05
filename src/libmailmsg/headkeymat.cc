/* headkeymat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on mail-message header keys */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This module was originally written.  This was written as
	part of the PCS mailer code cleanup!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	headkeymat

	Description:
	Given a mail-message header key name 'key' such as "Subject",
	determine if it is present in the user supplied string given
	as 'sp'.  Return 0 if there is no match, else we return
	the character position of the start of the header value
	string.  The match is case independent.

	Synopsis:
	int headkeymat(cc *key,cc *sp,int sl) noex

	Arguments:
	key		key name (c-string) to test for
	sp		c-string to match pointer
	sl		c-string to match length

	Returns:
	>0		the key was found and the position of the 
			value (not the key) in the user supplied
			string under test is returned
	==0		the key was not found
	<0		error code (system-return)

	Notes:
	1. Reminder that header-key names MUST start in column
	zero ('0') with no leading white-space characters (at all).
	Historically, white-space WAS allowed AFTER the header-key
	name and before the colon (':') character.

	See-also:
	hmatch(3pcs)
	mheader(3pcs)
	mailmsgmathdr(3mailmsg)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstrings>		/* CSTD |strncasecmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<six.h>			/* LIBUC |sichr(3uc)| */
#include	<rmx.h>			/* LIBUC |rmblk(3uc)| */
#include	<strwcmp.h>		/* LIBUC */
#include	<toxc.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"headkeymat.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	TOUC(ch)	CHAR_TOUC(ch)


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */

local inline bool isbasic(cc *keyp,int keyl,cc *hbuf,int hlen) noex {
    	bool f = (keyl > 0) && (hlen > 0) ;
	if (f) {
       	    f = (TOUC(keyp[0]) == TOUC(hbuf[0])) ;
	}
    	return f ;
} /* end subroutine (isbasic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int headkeymat(cchar *keyp,cchar *hbuf,int µhlen) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ; /* return-value */
	if (cint keyl = getlenstr(keyp) ; keyl >= 0) {
	    if (cint hlen = getlenstr(hbuf,µhlen) ; hlen >= 0) {
	        rs = SR_OK ;
		if (isbasic(keyp,keyl,hbuf,hlen)) {
	            if (cint si = sichr(hbuf,hlen,':') ; si >= keyl) {
			if (cint kl = rmblk(hbuf,si) ; kl == keyl) {
		            if (strncasecmp(hbuf,keyp,keyl) == 0) {
			        idx = (si + 1) ;
		            }
		        } /* end if (rmblk) */
	            } /* end if (had ':') */
		} /* end if (basic requirement) */
	    } /* end if (getlenstr) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? idx : rs ;
} /* end subroutine (headkeymat) */


