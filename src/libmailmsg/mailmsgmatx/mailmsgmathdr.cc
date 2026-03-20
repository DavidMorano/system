/* mailmsgmathdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on a message header (returns the key-name) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	I copied this from something that I wrote previously.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsgmathdr

	Description:
	This subroutine tests whether a MSG-header is in the supplied
	test string.  If a MSG-header is present, then the subroutine
	returns the length of the MSG-header key, otherwise it
	returns zero (0).  the string-index to the beginning of
	the hdr-key is returned in the variable pointed to be 'vip'
	argument.

	Synopsis:
	int mailmsgmathdr(cchar *sp,int sl,int *vip) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length
	vip		pointer to result string-index of hdr-key (optional)

	Returns:
	>0		yes-matched: length of found header-key
	==0		no-match: did not get a match
	<0		error code (system-return)

	Noes:
	1. Reminder that header-key names MUST start in column
	zero ('0') with no leading white-space characters (at all).
	Historically, white-space WAS allowed AFTER the header-key
	name and before the colon (':') character.
	2.  The 'ALT1' compile-time switch (above) was an experiment
	to try to find a faster way to execute this subroutine.
	Profiling shows that this subroutine (as we might expect
	already) is significant in the total performance of reading
	large mailboxes (the only really performance challenge this
	whole program has).  So far, no significant way to increase
	performance within this subroutine has been found (so we
	leave the compile-switch OFF).

	See-also:
	hmatch(3pcs)
	mheader(3pcs)
	headkeymat(3mailmsg)
	mailmsgmathdr(3mailmsg)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mailmsgmathdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	ISBLK(ch)	isblk(ch)
#define	ISALP(ch)	CHAR_ISALPHA(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline bool isblk(int ch) noex {
        return ((ch == ' ') || (ch == '\t')) ;
}

local inline bool iskey(int ch) noex {
        return ISALP(ch) || (ch == '-') || (ch == '_') ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgmathdr(cchar *sp,int µsl,int *vip) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    rs = SR_OK ;
	    if ((sl > 0) && ISALP(sp[0])) {
	        int	tl = sl ;
	        cchar	*tp = sp ;
	        while (tl && *tp && iskey(*tp) && (*tp != ':')) {
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	        kl = intconv(tp - sp) ;
	        while (tl && ISBLK(*tp)) {
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	        if (tl && (*tp == ':')) {
	            if (vip) *vip = intconv((tp + 1) - sp) ;
		} else {
		    kl = 0 ; /* <- signal no-match */
		}
	    } /* end if (required) */
	} /* end if (gelenstr) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mailmsgmathdr) */


