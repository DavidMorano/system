/* hmatch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* header key match */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This is part of my cleanup-compatibility effort.

	= 1998-11-22, David A­D­ Morano
        I did some more clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	hmatch

  	Description:
	Is the initial non-white-space substring of the given
	c-string to be tested the specified 'header' string?  Return
	0 if there is no match, else we return the character position
	of the header-value string.  The match is case independent.

	Synopsis:
	int hmatch(cchar *header,cchar *str) noex

	Arguments:
	- header	header string that we are looking for
	- str		string value to test for header

	Returns:
	>0		index to first non-white-space header-value character
	==0		no header match
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<char.h>
#include	<localmisc.h>

#include	"hmatch.h"


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

int hmatch(cchar *header,cchar *str) noex {
    	int		rs = SR_FAULT ;
	int		rc = 0 ; /* return-value */
	if (header && str) {
	    cchar	*hp = header ;
	    cchar	*sp = str ;
	    bool	fmat = false ;
	    rs = SR_OK ;
	    while (CHAR_ISWHT(*sp)) {
		sp += 1 ;
	    }
	    while (*sp && *hp && (*sp != ':') && (! CHAR_ISWHT(*sp))) {
	        fmat = (CHAR_TOLC(*sp) == CHAR_TOLC(*hp)) ;
	        if (! fmat) break ;
	        sp += 1 ;
	        hp += 1 ;
	    } /* end while */
	    if (fmat) {
		fmat = (hp[0] == '\0') ; /* all of header-key matched */
	    }
	    if (fmat) {
	        while (CHAR_ISWHT(*sp)) {
		    sp += 1 ;
	        }
	        if (*sp++ == ':')  {
	            while (CHAR_ISWHT(*sp)) {
			sp += 1 ;
		    }
	            rc = intconv(sp - str) ;
	        } /* end if (have colon) */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (hmatch) */


