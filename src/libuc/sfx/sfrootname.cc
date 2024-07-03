/* sfrootname SUPPORT */
/* lang=C++20 */

/* string-find root-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfrootname

	Description:
	Find the root-name from a program-root.

	Synopsis:
	int sfrootname(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		string
	sl		string-length
	rpp		pointer to result pointer

	Returns:
	-		length of found string

	Examples:
	source c-string:	/usr/add-on/pcs/bin/pcsv
	root-name is:		pcs

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* <- not needed */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfrootname(cchar *sp,int sl,cchar **rpp) noex {
	int		cl = -1 ;
	if (sp && rpp) {
	    cl = sfbasename(sp,sl,rpp) ;
	}
	return cl ;
}
/* end subroutine (sfrootname) */


