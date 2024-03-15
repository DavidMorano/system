/* rmleadzero SUPPORT */
/* lang=C++20 */

/* remove leading white-space and zero characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	rmleadzero

	Description:
	Remove leading white-space and zero characters from a c-string.

	Synopsis:
	int rmleadzero(cchar *sp,int sl) noex

	Arguments:
	sp		c-string buffer
	sl		c-string length

	Returns:
	-		length of remaining c-string after leading removal

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<utypedefs.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"rmleadzero.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int rmleadzero(cchar *sp,int sl) noex {
	auto isx = [] (int ch) -> bool {
	    bool	f = false ;
	    ch &= UCHAR_MAX ;
	    f = f || iszero(ch) ;
	    f = f || iswhite(ch) ;
	    return f ;
	} ;
	int	nsl = sl ;
	for (int i = 0 ; (i < (sl-1)) && isx(sp[0]) ; i += 1) {
	    nsl -= 1 ;
	} /* end for */
	return nsl ;
}
/* end subroutine (rmleadzero) */


