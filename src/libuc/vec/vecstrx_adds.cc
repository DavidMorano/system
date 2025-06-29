/* vecstrx_adds SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* add white-space separated substrings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_adds

	Description:
	This subroutine adds white-space separated substrings of a
	given string to the object.

	Synopsis:
	int vecstrx_adds(vecstrx *op,cchar *sp,int sl) noex

	Arguments:
	op		pointer to VECSTR object
	sp		string to parse for substrings
	sl		length of string to parse for substrings

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstrx::adds(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (sp) {
	    int		wl ;
	    cchar	*wp ;
	    rs = SR_OK ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    while ((sl > 0) && ((wl = sfnext(sp,sl,&wp)) > 0)) {
	        c += 1 ;
	        rs = add(wp,wl) ;
	        sl -= intconv((wp + wl) - sp) ;
	        sp = (wp + wl) ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_adds) */


