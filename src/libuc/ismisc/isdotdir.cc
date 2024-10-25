/* isdotdir SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* is a directory entry a dot-directory? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	isfotdir

	Description:
	This subroutine determines if a directory entry is a
	dot-directory.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"ismisc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isdotdir(cchar *dname) noex {
	int		f = false ;
	if (dname[0] == '.') {
	    f = f || (dname[1] == '\0') ;
	    f = f || ((dname[1] == '.') && (dname[2] == '\0')) ;
	}
	return f ;
}
/* end subroutine (isdotdir) */


