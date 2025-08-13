/* ucmkdir SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* like |mkdir(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_mkdir

	Description:
	Make a direcroy with a given file-mode.

	Symopsis:
	int uc_mkdir(cchar *dname,mode_t dm) noex

	Arguments:
	dname		directory name (c-string) to make
	dm		directory mode to us (UMASK applies)

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<mkx.h>
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

int uc_mkdir(cchar *fname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) {
	            if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		        rs = u_mkdir(ebuf,dm) ;
		        rc = rs ;
	            } else if (rs == 0) {
		        rs = u_mkdir(fname,dm) ;
		        rc = rs ;
	            }
	            rs1 = libmalloc_free(ebuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (uc_mkdir) */


