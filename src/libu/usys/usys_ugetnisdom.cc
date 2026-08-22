/* usys_ugetnisdom SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ugetnisdom

	Description:
	This provides a means to get the NIS domain-name from the
	operating system in an OS-independent way by regular callers.

	Synosis:
	int ugetnisdom(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (system-return)

	Notes:
	What does each OS take as a buffer length?
	OS			type		caveats
	-----------------------------------------------------
	+ POSIX®		size_t
	+ Linux			size_t
	+ Solaris®		int		contraru to online man-pages
	+ Darwin (macOS)	int


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |getdomainname(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */

#include	"usys_ugetnisdom.h"

#if	F_SUNOS || F_DARWIN

local unixret_t getdom(char *rbuf,int rlen) noex {
    	return getdomainname(rbuf,rlen) ;
} /* end subroutine */

#else /* all other OSes */

local unixret_t getdom(char *rbuf,int rlen) noex {
    	csize	rsize = size_t(rlen) ;
    	return getdomainname(rbuf,rsize) ;
} /* end subroutine */

#endif /* which operating system */


namespace libu {
    sysret_t ugetnisdom(char *rbuf,int rlen) noex {
        cint		maxlen = (2*1024) ; /* pseudo-random value */
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        if (rlen > maxlen) rlen = maxlen ;
	        if ((rs = getdom(rbuf,rlen)) >= 0) {
	            rbuf[rlen] = '\0' ; /* for safety */
	        } else {
	            rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugetnisdom) */
} /* end namespace (libu) */


