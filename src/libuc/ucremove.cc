/* ucremove SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* remove a file */


/* revision history:

	= 1998-05-09, David A­D­ Morano
	This subroutine was writen for some reason.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_remove

  	Description:
	This subroutine removes a file or a directory.

	Synopsis:
	int uc_remove(cchar *fname) noex

	Arguments:
	fname		file-name

	Returns:
	>=0		OK
	<0		error (system-return)

	Updated note:
	Like the |rename(2)| call, this subroutine has nothing to
	do with STDIO either!  Some documentation thinks that it
	does (or did).  What is the facination with STDIO??  One
	would think that the sun rises and sets on it!  Also, I
	have no idea why this subroutine is written out (implemented)
	here.  Yes, I wrote this but I am not sure why.  We must
	not have had |remove(3c)| on some platform back in the bad
	'ole days somewhere.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


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

int uc_remove(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (ustat sb ; (rs = uc_stat(fname,&sb)) >= 0) {
	            if (S_ISDIR(sb.st_mode)) {
	                rs = u_rmdir(fname) ;
	            } else {
	                rs = u_unlink(fname) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_remove) */


