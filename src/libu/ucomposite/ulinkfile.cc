/* ulinkfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* link a file as it should be */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_linkfile

	Description:
	These subroutines provide various copy-file operations.

	Synopsis:
	int u_linkfile(cchar *sfn,cchar *dfn) noex

	Arguments:
	sfn	source file
	dfn	destination file

	Returns:
	>=0		number of bytes copied (up to INT_MAX)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"ulinkfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_linkfile(cchar *sfn,cchar *dfn) noex {
	int		rs = SR_FAULT ;
	if (sfn && dfn) ylikely {
	    rs = SR_INVALID ;
	    if (sfn[0] && dfn[0]) ylikely {
		cint	fdcwd = AT_FDCWD ;
		cint	fl = 0 ;
		rs = u_linkat(fdcwd,sfn,fdcwd,dfn,fl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_linkfile) */


/* local subroutines */


