/* strstdfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* BFILE standard-file-name determination */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strstdfname

	Description:
	Test and figure out if the caller intent was to really get
	the standard-input for a BFILE file-open operation.

	Synopsis:
	cchar *strstdfname(cchar *atf) noex

	Arguments:
	atf	file-name to check to see if a BFILE standard-input wanted

	Returns:
	-	a (possibly) new file-name that is recognized as BFILE STDIN

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"strstdfname.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *strstdfname(int w,cchar *atf) noex {
	if ((w >= 0) && (w < stdfile_overlast)) ylikely {
            if ((atf == nullptr) || (atf[0] == '\0') || (atf[0] == '-')) {
                atf = stdfnames[w] ;
	    }
	}
	return atf ;
}
/* end subroutine (strstdfname) */


