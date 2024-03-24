/* mailmsgmathdr SUPPORT */
/* lang=C++20 */

/* match on a message header (returns the key-name) */
/* version %I% last-modified %G% */

#define	CF_SPACETAB	1		/* header whitespace is space-tab */
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
	This subroutine tests whether a MSG-header is in the suppled
	test string.  If a MSG-header is present, then the subroutine
	returns the length of the MSG-header key, otherwise it
	returns zero (0).

	Synopsis:
	int mailmsgmathdr(cchar *ts,int tslen,int *ip) noex

	Arguments:
	ts		string to test for header
	tslen		length of input string
	ip		pointer to string-index of header-value

	Returns:
	>0		yes-matched: length of found header-key
	==0		no-match: did not get a match
	<0		error (system-return)

	Design notes:

	The 'ALT1' compile-time switch (above) was an experiment
	to try to find a faster way to execute this subroutine.
	Profiling shows that this subroutine (as we might expect
	already) is significant in the total performance of reading
	large mailboxes (the only really performance challenge this
	whole program has).  So far, no significant way to increase
	performance has been found (so we leave the compile-switch
	OFF).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mailmsgmathdr.h"


/* local defines */

#define	SPACETAB(c)	(((c) == ' ') || ((c) == '\t'))

#if	CF_SPACETAB
#define	HEADERWHITE(c)	SPACETAB(c)
#else
#define	HEADERWHITE(c)	CHAR_ISWHITE(c)
#endif

#ifndef	CF_ALT1
#define	CF_ALT1		0		/* use alternative-1 */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_alt = CF_ALT1 ;


/* exported variables */


/* exported subroutines */

int mailmsgmathdr(cchar *ts,int tslen,int *ip) noex {
	int		rs = SR_FAULT ;
	if (ts) {
	    int		tl ;
	    int		kl = 0 ;
	    cchar	*tp = ts ;
	    if (ip) {
	        *ip = 0 ;
	    }
	    if (tslen < 0) tslen = strlen(ts) ;
	    tl = tslen ;
	    if constexpr (f_alt) {
	        bool	f = false ;
	        while (tl) {
	            cint	ch = *tp ;
		    f = (ch == 0) ;
	            f = f || HEADERWHITE(ch) ;
	            f = f || (ch == ':') ;
		    if (f) break ;
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	    } else {
	        while (tl && *tp && (! HEADERWHITE(*tp)) && (*tp != ':')) {
	            tp += 1 ;
	            tl -= 1 ;
	        } /* end while */
	    } /* end if-constexpr (f_alt) */
	    kl = (tp - ts) ;
	    while (tl && HEADERWHITE(*tp)) {
	        tp += 1 ;
	        tl -= 1 ;
	    } /* end while */
	    if (ip) {
	        *ip = ((tp + 1) - ts) ;
	    }
	    rs = (tl && (*tp == ':')) ? kl : 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgmathdr) */


