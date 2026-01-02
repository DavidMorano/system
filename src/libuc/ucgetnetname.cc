/* uc_getnetname */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* get the Net-name of the current user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine retrieves (we will go with that word) the
	(so-called) net-name of the current user.

	Net names look like:
		unix.<uid>@<nisdomain>

	an example of which is:
		unix.201@rightcore.com

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	PROCNAME	"keyserv"	/* required process */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getnetname(cchar *) ;	/* UNIX® ONC interface */
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getnetname(char *nbuf) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (nbuf) {
	    const uid_t		uid = 0 ; /* root user */
	    if ((rs = uc_procpid(PROCNAME,uid)) > 0) {
	        if (getnetname(nbuf) > 0) {
	            len = strlen(nbuf) ;
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } else if (rs == 0) {
	        rs = SR_BUSY ;		/* was previously SR_UNAVAIL */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_getnetname) */


