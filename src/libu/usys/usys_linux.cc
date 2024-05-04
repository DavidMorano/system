/* usys_linux SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"usys_linux.h"

#if	defined(OSNAME_Linux) && (OSNAME_Linux > 0)


int linux_ugetnisdom(char *rbuf,int rlen) noex {
	csize		rsz = size_t(rlen+1) ;
	int		rs ;
	if ((rs = getdomainname(rbuf,rsz)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (linux_ugetnisdom) */


#else /* other operating systems */


int linux_ugetnisdom(char *rbuf,int rlen) noex {
	int		ec = EFAULT ;
	if (rbuf) {
	    ec = EINVAL ;
	    if (rlen >= 0) {
		ec = ENOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return (- ec) ;
}
/* end subroutine (linux_ugetnisdom) */


#endif /* defined(OSNAME_Linux) && (OSNAME_Linux > 0) */
/* USYS_LINUX finish */


