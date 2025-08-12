/* uc_access SUPPORT */
/* charset=ISO8859-1 */

/* interface component for UNIX® library-3c */
/* POSIX file-unlink */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mkx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int uc_access(cchar *fname,int am) noex {
	cint		elen = MAXPATHLEN ;
	int		rs ;
	char		*ebuf ;

	if ((rs = uc_libmalloc((elen+1),&ebuf)) >= 0) {
	    if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		rs = u_access(ebuf,am) ;
	    } else if (rs == 0) {
		rs = u_access(fname,am) ;
	    }
	    uc_libfree(ebuf) ;
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (uc_access) */


