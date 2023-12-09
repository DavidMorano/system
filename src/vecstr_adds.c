/* vecstr_adds */
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
	vecstr_adds

	Description:
	This subroutine adds white-space separated substrings of a
	given string to the object.

	Synopsis:
	int vecstr_adds(vecstr *op,cchar *sp,int sl) noex

	Arguments:
	op		pointer to VECSTR object
	sp		string to parse for substrings
	sl		length of string to parse for substrings

	Returns:
	>=0		number of elements loaded
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"vecstr.h"


/* local defines */


/* external subroutines */

extern int	sfnext(cchar *,int,cchar **) noex ;


/* external variables */


/* forward references */


/* local structures */


/* local variables */


/* exported subroutines */

int vecstr_adds(vecstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && sp) {
	    int		wl ;
	    cchar	*wp ;
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    while ((sl > 0) && ((wl = sfnext(sp,sl,&wp)) > 0)) {
	        c += 1 ;
	        rs = vecstr_add(op,wp,wl) ;
	        sl -= ((wp + wl) - sp) ;
	        sp = (wp + wl) ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_adds) */


