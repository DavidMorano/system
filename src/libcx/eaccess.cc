/* eaccess SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check file access for the current process by its effective UID */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	eaccess

	Description:
	Check access by effective UID.

	Synopsis:
	int eaccess(cchar *fname,int am)

	Arguments:
	fname		file-name to check
	am		access mode

	Returns:
	>=0		OK
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

unixret_t eaccess(cchar *fname,int am) noex {
    	const int	fdcwd = AT_FDCWD ;
    	const int	fl = AT_EACCESS ;
	return faccessat(fdcwd,fname,am,fl) ;
}
/* end subroutine (eaccess) */


