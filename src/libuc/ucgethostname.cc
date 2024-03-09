/* ucgethostname SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get the "hostname" (really the "nodename") of the current machine */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the 'hostname' of the current machine.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* exported variables */


/* exported subroutines */

int uc_gethostname(char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	if (hbuf) {
	    rs = SR_INVALID ;
	    hbuf[0] = '\0' ;
	    if (hlen > 0) {
	        if ((rs = gethostname(hbuf,(hlen+1))) >= 0) {
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


