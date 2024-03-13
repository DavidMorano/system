/* usys_getrandom SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

	Important note:

	On Darwin (MacOS) the header 'sys/random.h' -- which is
	supposed to contain the declaration for the subroutine
	|getentropy(2)| --  is broken when using GCC (G++) as the
	compiler.  It figures.  So I declared the |getentropy(2)|
	subroutine myself below.  This cr*p gets very old after a
	while.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* GETRANDOM start */
#if	(!defined(SYSHAS_GETRANDOM)) || (SYSHAS_GETRANDOM == 0)

#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"usys_getrandom.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getrandom(void *rbuf,size_t rlen,uint) noex {
	int		rc = 0 ;
	int		rl = 0 ;
	if (rbuf) {
		csize		inc = 256 ;
		const caddr_t	ca = caddr_t(rbuf) ;
		while ((rc >= 0) && (rlen > 0)) {
		    csize	ml = min(rlen,inc) ;
		    if ((rc = getentropy((ca+rl),ml)) >= 0) {
			rl += rc ;
			rlen -= rc ;
		    } /* end if */
	 	} /* end while */
	} else {
	    rc = -1 ;
	    errno = EFAULT ;
	} /* end if (non-null) */
	return (rc >= 0) ? rl : rc ;
}

#endif /* (!defined(SYSHAS_GETRANDOM)) || (SYSHAS_GETRANDOM == 0) */
/* GETRANDOM end */


