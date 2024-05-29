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
#include	<climits>
#include	<unistd.h>
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usupport.h>

#include	"usysauxinfo_darwin.h"


/* SYSAUXINFO begin */
#if	defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0)

namespace usys {
    using namespace	usys ;
    sysret_t usysauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    cchar	*vp = nullptr ;
	    rs = SR_NOENT ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
	 	vp = "x86_64" ;
		break ;
	    case SAI_PLATFORM:
	 	vp = "Mac-Mini" ;
		break ;
	    case SAI_HWPROVIDER:
	 	vp = "Apple" ;
		break ;
	    } /* end switch */
	    if (vp) {
		rs = sncpy(rbuf,rlen,vp) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (usysauxinfo) */
}

#endif /* defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0) */
/* SYSAUXINFO end */


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSAUXINFO_DARWIN finish */


