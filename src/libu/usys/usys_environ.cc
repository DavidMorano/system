/* usys_environ SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Environment-Variable support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_environ

	Description:
	This file contains some support for retrieving the process
	enivronment string array pointer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysflag.h>
#include	<usysrets.h>

#include	"usys_environ.hh"


local int nenv(mainv envv) noex {
    	int ne = 0 ;
	while (envv[ne]) ne += 1 ;
    	return ne ;
} /* end subroutine (nenv) */

/* UUSYSENVIRON start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
    
#include <crt_externs.h>

namespace libu {
    int usys_environ(mainv *rp) noex {
    	int	rs = SR_FAULT ;
	if (rp) {
	    rs = SR_NOMSG ;
	    if (char ***ptr = _NSGetEnviron() ; ptr) {
	        *rp = mainv(*ptr) ;
	        rs = nenv(*rp) ;
	    }
	} /* end if (non-null) */
 	return rs ;
    } /* end subroutine (usys_environ) */
} /* end namespace (libu) */

#else /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */

namespace libu {
    int usys_environ(mainv *rp) noex {
    	int	rs = SR_FAULT ;
	if (rp) {
	    rs = SR_NOMSG ;
	    if (environ) {
	        *rp = mainv(environ) ;
	        rs = nenv(*rp) ;
	    }
	} /* end if (non-null) */
 	return rs ;
    } /* end subroutine (usys_environ) */
} /* end namespace (libu) */

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* UUSYSENVIRON end */


