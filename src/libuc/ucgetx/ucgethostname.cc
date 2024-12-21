/* ucgethostname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the "hostname" (really the "nodename") of the current machine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_gethostname

	Description:
	This subroutine gets the 'hostname' of the current machine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
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

int uc_gethostid(ulong *lp) noex {
	return u_gethostid(lp) ;
}
/* end subroutine (uc_gethostid) */

int uc_gethostname(char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	if (hbuf) {
	    rs = SR_INVALID ;
	    hbuf[0] = '\0' ;
	    if (hlen > 0) {
		csize	hsz = size_t(hlen + 1) ;
	        if ((rs = gethostname(hbuf,hsz)) >= 0) {
	            hbuf[hlen] = '\0' ;
	            rs = strlen(hbuf) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gethostname) */


