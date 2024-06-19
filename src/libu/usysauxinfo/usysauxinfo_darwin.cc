/* usysauxinfo_darwin SUPPORT */
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

/* USYSAUXINFO_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<sys/sysctl.h>		/* <- Darwin |sysctl(3c)| */
#include	<unistd.h>
#include	<climits>
#include	<cerrno>
#include	<new>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<intsat.h>

#include	"usysauxinfo_darwin.h"

using namespace	libu ;

namespace usysauxinfo {
    sysret_t ugetauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    cchar	*vp = nullptr ;
	    cchar	*name = nullptr ;
	    rs = SR_OK ;
	    rbuf[0] = '\0' ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
		name = "hw.machine" ;
		break ;
	    case SAI_MACHINE:
		name = "machdep.cpu.brand_string" ;
		break ;
	    case SAI_PLATFORM:
		name = "hw.model" ;
		break ;
	    case SAI_HWPROVIDER:
	 	vp = "Apple" ;
		break ;
	    case SAI_HWSERIAL:
		rs = loadhostid(rbuf,rlen) ;
		break ;
	    case SAI_RPCDOMAIN:
		name = "kern.nisdomainname" ;
		break ;
	    case SAI_UUID:
		name = "kern.uuid" ;
		break ;
	    default:
		rs = SR_NOTFOUND ;
		break ;
	    } /* end switch */
	    if (rs >= 0) {
		if (name) {
		    rs = darwin_usysctl(rbuf,rlen,name) ;
	        } else if (vp) {
		    rs = sncpy(rbuf,rlen,vp) ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugetauxinfo) */
}

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSAUXINFO_DARWIN finish */


