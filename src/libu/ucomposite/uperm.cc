/* uperm SUPPORT */
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
	u_perm

	Description:
	These subroutines provide various copy-file operations.

	Synopsis:
	int u_perm(cchar *fn,int am) noex

	Arguments:
	fn	source file
	am	access-mode

	Returns:
	>=0		OK
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

#include	"uperm.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_perm(cchar *fn,int am) noex {
	cint	fdcwd = AT_FDCWD ;
	return u_atperm(fdcwd,fn,am) ;
} /* end subroutine (u_perm) */

namespace libu {
    int uperm(cchar *fn,int am) noex {
	return u_perm(fn,am) ;
    }
} /* end namespace */


/* local subroutines */


