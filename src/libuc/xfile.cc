/* xfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* eXecutable File? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	xfile

	Description:
	This subroutine tests tests if a given file is executable
	(has its 'x' access permission mode bit set).  For the
	present purposes, an "executable" file is also a "regular"
	file, as opposed to a socket or pipe, et cetera.

	Synopsis:
	int xfile(ids *idp,cchar *fname) noex

	Arguments:
	idp		pointer to IDS object
	fname		name of file to check

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucfileop.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"xfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int xfile(ids *idp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (idp && fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        if (ustat sb ; (rs = uc_stat(fname,&sb)) >= 0) ylikely {
	            if (S_ISREG(sb.st_mode)) {
		        rs = permids(idp,&sb,X_OK) ;
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (uc_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (xfile) */


