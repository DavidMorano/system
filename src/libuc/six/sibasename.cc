/* sibasename SUPPORT */
/* lang=C++20 */

/* get the base file name out of a file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name
	sibasename

	Description:
	This routine returns the index within the given string to
	the start of the base-name portion of the given path string.

	Synopsis:
	int sibasename(cchar *sp,int sl) noex

	Arguments:
	sp	given path string
	sl	length of given path string (can be -1)

	Returns:
	+	index of found string

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sibasename(cchar *sp,int sl) noex {
	int		si{} ; /* used-afterwards */
	if (sl < 0) sl = strlen(sp) ;
	while ((sl > 0) && (sp[sl - 1] == '/'))  {
	    sl -= 1 ;
	}
	for (si = sl ; si > 0 ; si -= 1) {
	    if (sp[si - 1] == '/') break ;
	}
	return si ;
}
/* end subroutine (sibasename) */


