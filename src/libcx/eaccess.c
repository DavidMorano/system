/* eaccess */

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
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<xperm.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int eaccess(cchar *fname,int am) noex {
	return perm(fname,-1,-1,nullptr,am) ;
}
/* end subroutine (eaccess) */


