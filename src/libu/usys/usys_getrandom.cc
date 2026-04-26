/* usys_getrandom SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<cstddef>
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>

#include	"usys_getrandom.h"

#define	MAXRANDSIZE	256		/* limit imposed (else EIO) */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */

unixret_t getrandom(void *rbuf,size_t rsz,uint) noex {
	unixret_t	rc = 0 ;
	unixret_t	rl = 0 ; /* return-value */
	if (rbuf) {
	    if (rsz > 0) {
		csize		inc = MAXRANDSIZE ;
		const caddr_t	ca = caddr_t(rbuf) ;
		while ((rc >= 0) && (rsz > 0)) {
		    csize msz = min(rsz,inc) ;
		    if ((rc = getentropy((ca+rl),msz)) >= 0) {
			rl += intconv(msz) ;
			rsz -= msz ;
		    } /* end if */
	 	} /* end while */
	    } /* end if (non-negative) */
	} else {
	    rc = -1 ;
	    errno = EFAULT ;
	} /* end if (non-null) */
	return (rc >= 0) ? rl : rc ;
} /* end subroutine (getrandom) */


#endif /* (!defined(SYSHAS_GETRANDOM)) || (SYSHAS_GETRANDOM == 0) */
/* GETRANDOM end */


