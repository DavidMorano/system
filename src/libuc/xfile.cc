/* xfile SUPPORT */
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
	We test if a given file is executable (has its 'x' access
	permission mode bit set).

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
#include	<sys/stat.h>
#include	<usystem.h>
#include	<ids.h>
#include	<xperm.h>
#include	<localmisc.h>

#include	"xfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int xfile(ids *idp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (idp && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        USTAT	sb ;
	        if ((rs = uc_stat(fname,&sb)) >= 0) {
	            if (S_ISREG(sb.st_mode)) {
		        rs = sperm(idp,&sb,X_OK) ;
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (uc_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (xfile) */


