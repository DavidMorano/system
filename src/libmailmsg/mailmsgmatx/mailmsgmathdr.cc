/* mailmsgmathdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on a message header (returns the key-name) */
/* version %I% last-modified %G% */

#define	CF_ALT1		0		/* use alternative-1 */

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
	returns zero (0).

	Synopsis:
	int mailmsgmathdr(cchar *sp,int sl,int *ip) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length
	ip		pointer to string-index of header-value

	Returns:
	>0		yes-matched: length of found header-key
	==0		no-match: did not get a match
	<0		error code (system-return)

	Design notes:
	The 'ALT1' compile-time switch (above) was an experiment
	to try to find a faster way to execute this subroutine.
	Profiling shows that this subroutine (as we might expect
	already) is significant in the total performance of reading
	large mailboxes (the only really performance challenge this
	whole program has).  So far, no significant way to increase
	performance within this subroutine has been found (so we
	leave the compile-switch OFF).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"mailmsgmathdr.h"

import libutil ;

/* local defines */

#ifndef	CF_ALT1
#define	CF_ALT1		0		/* use alternative-1 */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline bool ishdrwht(int ch) noex {
        return ((ch == ' ') || (ch == '\t')) ;
}


/* local variables */

constexpr bool		f_alt = CF_ALT1 ;


/* exported variables */


/* exported subroutines */

int mailmsgmathdr(cchar *sp,int sl,int *ip) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    int		tl ;
	    int		kl = 0 ;
	    cchar	*tp = sp ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (ip) {
	        *ip = 0 ;
	    }
	    tl = sl ;
	    if_constexpr (f_alt) {
	        bool	f = false ;
	        while (tl) {
	            cint	ch = *tp ;
		    f = (ch == 0) ;
	            f = f || ishdrwht(ch) ;
	            f = f || (ch == ':') ;
		    if (f) break ;
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	    } else {
	        while (tl && *tp && (! ishdrwht(*tp)) && (*tp != ':')) {
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	    } /* end if_constexpr (f_alt) */
	    kl = intconv(tp - sp) ;
	    while (tl && ishdrwht(*tp)) {
	        tp += 1 ;
	        tl -= 1 ;
	    } /* end while */
	    if (ip) {
	        *ip = intconv((tp + 1) - sp) ;
	    }
	    rs = (tl && (*tp == ':')) ? kl : 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgmathdr) */


