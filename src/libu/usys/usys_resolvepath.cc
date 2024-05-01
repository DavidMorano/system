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

/* GETRANDOM start */
#if	(!defined(SYSHAS_GETRANDOM)) || (SYSHAS_GETRANDOM == 0)

#include	<climits>		/* |PATH_MAX| + |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"usys_resolvepath.h"


/* local defines */

#ifndef	PATH_MAX
#define	PATH_MAX	(4 * 1024)	/* maximum seen in the wild */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr int		maxpath = PATH_MAX ;


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
		    rl = strlen(rp) ;
		    if (rl <= rlen) {
			strcpy(rbuf,rp) ;
		    } else {
			rc = -1 ;
			errno = E2BIG ;
		    }
		    free(rp) ;
		} else {
		    rc = -1 ;
		} /* end if (realpath) */
	    } else {
		rc = -1 ;
		errno = EINVAL ;
	    } /* end if (non-negative) */
	} else {
	    rc = -1 ;
	    errno = EFAULT ;
	} /* end if (non-null) */
	return (rc >= 0) ? rl : rc ;
}

#endif /* (!defined(SYSHAS_GETRANDOM)) || (SYSHAS_GETRANDOM == 0) */
/* GETRANDOM end */


