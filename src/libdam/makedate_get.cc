/* makedate_get SPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the name on the MAKEDATE string */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	makedate_get

  	Description:
	We get the date out of the "makedate" ID string.

	Synopsis:
	int makedate_get(cchar *,cchar **) noex

	Arguments:
	makedate	pointer to the MAKEDATE string
	rpp		pointer to pointer to hold result

	Returns:
	<0		error
	>=		length of result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<char.h>		/* |CHAR_ISWHUTE(3cu)| */
#include	<ischarx.h>		/* |isdigitlatin(3uc)| */
#include	<localmisc.h>

#include	"makedate_get.h"


/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int makedate_get(cchar *md,cchar **rpp) noex {
    	cnullptr	np{} ;
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (md) {
	    rs = SR_NOTFOUND ;
	    if (cchar *cp ; (cp = strchr(md,CH_RPAREN)) != np) {
		rs = SR_OK ;
	        if (rpp) {
	            *rpp = nullptr ;
	        }
	        while (ISWHT(*cp)) {
	            cp += 1 ;
	        }
		{
	            cint ch = mkchar(*cp) ;
	            if (! isdigitlatin(ch)) {
	                while (*cp && (! ISWHT(*cp))) {
		            cp += 1 ;
	                }
	                while (ISWHT(*cp)) {
		            cp += 1 ;
	                }
	            } /* end if (skip over the name) */
		} /* end block */
		{ 
		    cchar *sp = cp ;
	            if (rpp) *rpp = cp ;
	            while (*cp && (! ISWHT(*cp))) {
	                cp += 1 ;
	            }
	            rl = intconv(cp - sp) ;
		} /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (makedate_get) */


