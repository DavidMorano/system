/* ucgethostname SUPPORT */
/* charset=ISO8859-1 */
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
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |u_gethostid(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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
} /* end subroutine (uc_gethostid) */

int uc_gethostname(char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	if (hbuf) {
	    rs = SR_INVALID ;
	    hbuf[0] = '\0' ;
	    if (hlen > 0) {
		csize	hsize = size_t(hlen + 1) ;
	        if ((rs = gethostname(hbuf,hsize)) >= 0) {
	            hbuf[hlen] = '\0' ;
	            rs = lenstr(hbuf) ;
	        } else {
	            rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_gethostname) */


