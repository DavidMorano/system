/* instrrev SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reverse the characters in a c-string in-place */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	instrrev

	Description:
	Reverse the characters of a string in-place.

	Synopsis:
	int instrrev(char *bp,int bl) noex

	Arguments:
	rp		source and destination buffer
	rl		length of source string

	Returns:
	>=0		length of result
	<0		error (system-return)


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"instrrev.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline void swapends(char *bp,int bl,int i) noex {
	cchar chr = bp[i] ;
	bp[i] = bp[bl-i-1] ;
	bp[bl-i-1] = chr ;
} /* end subroutine (swapends) */


/* local variables */


/* exported variables */


/* exported subroutines */

int instrrev(char *bp,int bl) noex {
	int		rs = SR_FAULT ;
	if (bp) ylikely {
	    rs = SR_OK ;
	    if (bl < 0) bl = lenstr(bp) ;
	    for (int i = 0 ; i < (bl/2) ; i += 1) {
		swapends(bp,bl,i) ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (instrrev) */


