/* makedirs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make directories (within a single path) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This code was originally written.

	= 2025-01-08, David A­D­ Morano
	I was (at first) just reviewing this code for C++20
	conformance, and I ended up just rewriting it essentually
	completely.  I did not intend to rewrite it.  The original
	code was not really bad.  But this function (subroutine)
	was so similar to |mkdirs(3uc)| that I lost control of
	myself and just adapted this function to use the |mkdirs(3uc)|
	subroutine instead.  We will see how this works out over
	time.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	makedirs

	Description:
	This subroutine creates a directory and all of its parent
	directories if they do not exist, and if it is possible to
	create them.

	Synopsis:
	int makedirs(cchar *dirp,int dirl,mode_t dm) noex

	Arguments:
	dirp		path to the directory to create
	dirl		length of directory path string
	dm		the file permission mode to create the directories

	Returns:
	>0		directory was created
	0		directory already existed
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mkdirs.h>
#include	<strnul.hh>
#include	<localmisc.h>

#include	"makedirs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int makedirs(cchar *dirp,int dirl,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (dirp) {
	    rs = SR_INVALID ;
	    if (dirp[0]) {
	        strnul	ds(dirp,dirl) ;
		rs = mkdirs(ds,dm) ;
		c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (makedirs) */


