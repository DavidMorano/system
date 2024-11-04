/* usysauxinfo_linux SUPPORT */
/* encoding=ISO8859-1 */
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

#include	<envstandards.h>	/* MUST be ordered first to configure */

/* USYSAUXINFO_LINUX start */
#if	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>

#include	"usysauxinfo_linux.h"

using namespace	libu ;

extern ugetnisdom(char *,int) noex ;

namespace libu {
    extern sysret_t loadhostid(char *,int) noex ;
}

namespace usysauxinfo {
    sysret_t ugetauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    ccar	*vp = nullptr ;
	    rs = SR_NOENT ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
	 	vp = "x86_64" ;
		break ;
	    case SAI_MACHINE:
	 	vp = "Intel(R) Core(TM) i7" ;
		break ;
	    case SAI_PLATFORM:
	 	vp = "Mac-Mini" ;
		break ;
	    case SAI_HWPROVIDER:
	 	vp = "Apple" ;
		break ;
	    case SAI_HWSERIAL:
		rs = loadhostid(rbuf,rlen) ;
		break ;
	    case SAI_RPCDOMAIN:
		rs = ugetnisdom(rbuf,rlen) ;
		break ;
	    } /* end switch */
	    if (vp) {
		rs = sncpy(rbuf,rlen,vp) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugetauxinfo) */
}

#endif /* defined(OSNAME_Linux) && (OSNAME_Linux > 0) */
/* USYSAUXINFO_LINUX finish */


