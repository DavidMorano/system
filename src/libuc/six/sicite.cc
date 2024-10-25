/* sicite SUPPORT */
/* lang=C++20 */

/* string-index to a citation escape */
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
	sicite

	Description:
	This subroutine finds the index up to a citation escape. The 
	caller must also supply an escape "name" that is also required
	to be matched in order to get a hit.

	Synopsis:
	int sicite(cchar *sp,int sl,cchar *ep,int el) noex

	Arguments:
	sp		source c-stirng to examine
	sl		source s-string length
	ep		escape c-string pointer
	el		escape c-string length

	Returns:
	-		index up to start of escape sequence

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>
#include	<toxc.h>
#include	<mkchar.h>
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

int sicite(cchar *sp,int sl,cchar *ep,int el) noex {
	int		cl = sl ;
	int		si = -1 ;
	cchar		*tp ;
	cchar		*cp = sp ;
	while ((tp = strnchr(cp,cl,'\\')) != nullptr) {
	    if (strncmp((tp + 1),ep,el) == 0) {
	        si = (tp - sp) ;
	        break ;
	    }
	    cl -= ((tp + 1) - cp) ;
	    cp = (tp + 1) ;
	} /* end while */
	return si ;
}
/* end subroutine (sicite) */


