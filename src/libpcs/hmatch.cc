/* hmatch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* header key match */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This is part of our cleanup-compatibility effort.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	hmatch

  	Description:
	Is the initial substring of 'buf' the specified 'header'
	string?  Return 0 if there is no match, else we return the
	character position of the header value string.  The match
	is case independent.

	Synopsis:
	int hmatch(cchar *header,cchar *sp) noex

	Arguments:
	- header	header string that we are looking for
	- str		string value to test for header

	Returns:
	>0		index to first non-white-space value character
	0		no header match
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
	    cchar	*bp = str ;
	    bool	f = false ;
	    while (*hp && (*hp != ':')) {
	        f = (CHAR_TOLC(*bp) != CHAR_TOLC(*hp)) ;
	        if (f) break ;
	        bp += 1 ;
	        hp += 1 ;
	    } /* end while */
	    if (! f) {
	        while (CHAR_ISWHT(*bp)) {
		    bp += 1 ;
	        }
	        if (*bp++ == ':')  {
	            while (CHAR_ISWHT(*bp)) bp += 1 ;
	            rc = intconv(bp - str) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (hmatch) */


