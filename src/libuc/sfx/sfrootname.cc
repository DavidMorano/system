/* sfrootname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-find root-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sfxname.h"		/* |sfbasename(3uc)| */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfrootname(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = -1 ;
	if (sp && rpp) ylikely {
	    rl = sfbasename(sp,sl,rpp) ;
	}
	return rl ;
} /* end subroutine (sfrootname) */


