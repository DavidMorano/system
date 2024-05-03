/* usys_resolvepath SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	For those operating systems that do not have |resolvepath(2)|
	we try to define it.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* RESOLVEPATH start */
#if	(!defined(SYSHAS_RESOLVEPATH)) || (SYSHAS_RESOLVEPATH == 0)

#include	<climits>		/* |PATH_MAX| + |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysflag.h>
#include	<utypedefs.h>

#include	"usys_resolvepath.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int resolvepath(cchar *fname,char *rbuf,size_t rsz) noex {
	cint		rlen = int(rsz & INT_MAX) ;
	int		rc = 0 ;
	int		rl = 0 ;
	if (rbuf) {
	    cnullptr	np{} ;
	    if (fname[0] && (rlen >= 0)) {
		 if (char *rp ; (rp = realpath(fname,np)) != np) {
		    if ((rl = strlen(rp)) <= rlen) {
			strcpy(rbuf,rp) ;
		    } else {
			rc = -1 ;
			errno = EOVERFLOW ;
		    }
		    free(rp) ;
		} else {
		    rc = -1 ;
		} /* end if (realpath) */
	    } else {
		rc = -1 ;
		errno = EINVAL ;
	    } /* end if (valid) */
	} else {
	    rc = -1 ;
	    errno = EFAULT ;
	} /* end if (non-null) */
	return (rc >= 0) ? rl : rc ;
}

#endif /* (!defined(SYSHAS_RESOLVEPATH)) || (SYSHAS_RESOLVEPATH == 0) */
/* RESOLVEPATH end */


