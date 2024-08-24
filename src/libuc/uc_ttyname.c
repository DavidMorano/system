/* ucttyname SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get the filename (path) of a terminal device */


/* revision history:

	= 1998-03-01, David A­D­ Morano
        This is written to get a portable (reentrant) version of TTYNAME.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the filepath of a terminal device under
	'/dev/'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>		/* |ttyname_r(3c)| */
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strnlen(3c)| */
#include	<usystem.h>
#include	<usysflag.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_ttyname(int fd,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
		rs = SR_INVALID ;
		if (dlen >= 0) {
		    if ((rs = ttyname_rp(fd,dbuf,dlen)) >= 0) {
	    	        len = strnlen(dbuf,dlen) ;
		    } else {
			rs = (- rs) ; /* returned an ERRNO code */
		    }
		} /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_ttyname) */


