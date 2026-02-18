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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<char.h>
#include	<localmisc.h>

#include	"hmatch.h"

#pragma		GCC dependency		"mod/pcsutil.ccm"

import pcsutil ;			/* |hdrmatbasic(3pcs)| */

/* local defines */

#define	ISWH(ch)	CHAR_ISBLANK(ch)
#define	TOUC(ch)	CHAR_TOUC(ch)


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
	int		idx = 0 ; /* return-value */
	if (header && str) {
	    rs = SR_OK ;
	    if (hdrmatbasic(header,str)) {
	        cchar	*hp = header ;
	        cchar	*sp = str ;
	        bool	fmat = false ;
	        while (*sp && *hp && (*sp != ':') && (! ISWH(*sp))) {
	            fmat = (TOUC(*sp) == TOUC(*hp)) ;
	            if (! fmat) break ;
	            sp += 1 ;
	            hp += 1 ;
	        } /* end while */
	        if (fmat) {
		    fmat = (hp[0] == '\0') ; /* all of header-key matched */
	        }
	        if (fmat) {
	            while (ISWH(*sp)) {
		        sp += 1 ;
	            }
	            if (*sp++ == ':')  {
	                while (ISWH(*sp)) {
			    sp += 1 ;
		        }
	                idx = intconv(sp - str) ;
	            } /* end if (have colon) */
	        } /* end if */
	    } /* end if (isbasic) */
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (hmatch) */


