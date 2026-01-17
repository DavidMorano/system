/* spwd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* define various sytem (global) variables */
/* last modified %G% vercion %I% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David A-D- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sqwd

	Description:
	I define some subroutines that are missing from some systems, who will
	remain nameless for now (Apple Darwin).

	Notes:
	1. The whole "shadow" password concept was created by
	Julianne (Julie) Frances Haugh II (in 1987 originally for
	SCO Xenix®).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>

#include	"spwd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)

#if	defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0)
/******************************************************************************/


/* *nothing-needed* */


/******************************************************************************/
#else /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */
/******************************************************************************/


namespace gnu {
    errno_t getspent_r(SPWD *spp,char *spbuf,size_t splen) noex {
	errno_t		ec = 0 ;
	if (spp && spbuf) {
	    memclear(spp) ;
	    if (splen > 0) {
	        ec = ENOSYS ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return ec ;
    } /* end subroutine (getspent_r) */
} /* end namespace (gnu) */


/******************************************************************************/
#endif /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */
#if	defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0)
/******************************************************************************/


/* *nothing-needed* */


/******************************************************************************/
#else /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */
/******************************************************************************/


namespace gnu {
    errno_t getspnam_r(cc *n,SPWD *spp,char *spbuf,int splen,CSPWD **rpp) noex {
	errno_t		ec = 0 ;
	if (spp && spbuf && n) {
	    memclear(spp) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOSYS ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) {
	    errno = ec ;
	    if (rpp) *rpp = nullptr ;
	}
	return ec ;
    } /* end subroutine (getspnam_r) */
} /* end namespace (gnu) */


/******************************************************************************/
#endif /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */
#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */
/******************************************************************************/


namespace gnu {
    void setspent() noex { }
    void endspent() noex { }
    SPWD *getspent() noex {
	errno = ENOSYS ;
	return nullptr ;
    }
    SPWD *getspnam(cchar *) noex {
	errno = ENOSYS ;
	return nullptr ;
    }
} /* end namespace (gnu) */

namespace gnu {
    errno_t getspent_r(SPWD *spp,char *spbuf,size_t splen,CSPWD **rpp) noex {
	errno_t		ec = 0 ;
	if (spp && spbuf) {
	    memclear(spp) ;
	    if (splen > 0) {
	        ec = ENOSYS ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) {
	    errno = ec ;
	    if (rpp) *rpp = nullptr ;
	}
	return ec ;
    } /* end subroutine (getspent_r) */
} /* end namespace (gnu) */

namespace gnu {
    errno_t getspnam_r(cc *n,SPWD *spp,char *spbuf,int splen,CSPWD **rpp) noex {
	errno_t		ec = 0 ;
	if (spp && spbuf && n) {
	    memclear(spp) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOSYS ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) {
	    errno = ec ;
	    if (rpp) *rpp = nullptr ;
	}
	return ec ;
    } /* end subroutine (getspnam_r) */
} /* end namespace (gnu) */


/******************************************************************************/
#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */


