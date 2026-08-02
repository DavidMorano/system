/* getfstype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the type of the filesystem for an FD */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-09-27, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getfstype

	Description:
	I return the file-system type (a c-string) associated with
	the file attached to the given file-descriptor.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |u_fstype(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"getfstype.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getfstype(char *nbuf,int nlen,int fd) noex {
	return u_fstype(fd,nbuf,nlen) ;
} /* end subroutine (getfstype) */


