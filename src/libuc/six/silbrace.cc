/* silbrace SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* is the next non-whatspace character a left-brace? */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-03-01, David A­D­ Morano
	This code was originally written.

	= 1998-09-01, David A­D­ Morano
	This subroutine was modified to process the way MMCITE does
	citation.  It used to use the old GNU 'lookbib' program in
	addition to the (old) standard UNIX version.  But neither
	of these are used now.  Straight out citeation keywrd lookup
	is done directly in a BIB database (files of which are
	suffixed 'rbd').

*/

/* Copyright © 1992,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sibrace

	Description:
	This subroutine checks to see if the next non-white-space
	character is a left-brace.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int silbrace(cchar *sp,int sl) noex {
	int		si = -1 ;
	if (sp) ylikely {
	    si = 0 ;
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	        si += 1 ;
	    }
	    if ((sl == 0) || (sp[0] != CH_LBRACE)) {
	        si = -1 ;
	    }
	} /* end if (non-null) */
	return si ;
}
/* end subroutine (silbrace) */


