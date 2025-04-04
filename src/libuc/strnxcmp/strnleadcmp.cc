/* strnleadcmp SUPPORT */
/* lang=C++20 */

/* check if string 's2' is a leading substring of string 's1' */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine returns true if 's2' is an initial substring
	of 's1'.  But only up to the maximum number of characters
	are checked.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<libutil.hh>
#include	<localmisc.h>

#include	"strnxcmp.h"


/* local defines */


/* external subroutines */


/* local varabiables */


/* exported variables */


/* exported subroutines */

int strnleadcmp(cchar *s1,cchar *s2,int lr) noex {
	int		f = true ;
	if (lr < 0) lr = xstrlen(s2) ;
	while (lr-- > 0) {
	    f = (*s2++ == *s1++) ;
	    if (! f) break ;
	} /* end while */
	return f ;
}
/* end subroutine (strnleadcmp) */


