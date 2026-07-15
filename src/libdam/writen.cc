/* writen SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* write to a network (classic function) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-02-21, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	writen

	Description:
	This is a classic function to write to a network (really a
	socket on a network) to ignore early returns.

	Synopsis:
	int writen(int fd,cvoid *nbuf,int nlen) noex

	Arguments:
	fd		file-descriptor
	nbuf		buffer of data to write pointer
	nlen		buffer of data to write length

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"writen.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int writen(int fd,cvoid *nbuf,int nlen) noex {
	return uc_writen(fd,nbuf,nlen) ;
} /* end subroutine (writen) */


