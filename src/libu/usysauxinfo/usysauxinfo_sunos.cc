/* usysauxinfo_sunos SUPPORT */
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

/* USYSAUXINFO_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

#include	<sys/types.h>
#include	<sys/systeminfo.h>	/* <- where the stuff we want is */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usupport.h>

#include	"usysauxinfo_sunos.h"

using namespace	libu ;

static sysret_t sunos_getauxinfo(char *,int,int) noex ;

namespace usysauxinfo {
    sysret_t ugetauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    int		r = -1 ;
	    rs = SR_NOENT ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
		r = SI_ARCHITECTURE ;
		break ;
	    case SAI_MACHINE:
		r = SI_MACHINE ;
		break ;
	    case SAI_PLATFORM:
		r = SI_PLATFORM ;
		break ;
	    case SAI_HWPROVIDER:
		r = SI_HW_PROVIDER ;
		break ;
	    case SAI_HWSERIAL:
		r = SI_HW_SERIAL ;
		break ;
	    case SAI_SRPCDOMAIN:
		r = SI_SRPC_DOMAIN ;
		break ;
	    } /* end switch */
	    if (r >= 0) {
		rs = sunos_getauxinfo(rbuf,rlen,r) ; /* Solaris® specific */
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugetauxinfo) */
}

static sysret_t sunos_getauxinfo(char *rbuf,int rlen,int req) noex {
	csize		rsz(rlen + 1) ;
	int		rs ;
	if ((rs = sysinfo(req,rbuf,rsz)) > rsz) {
	    rs = SR_OVERFLOW ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}

#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYSAUXINFO_SUNOS finish */


