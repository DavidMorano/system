/* spwd SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
/* last modified %G% vercion %I% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	I define some subroutines that are missing from some systems, who will
	remain nameless for now (Apple Darwin).


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"spwd.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)

#if	defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0)

int getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
	CSPWD	*rp{} ;
	int	rc = 0 ;
	errno = 0 ;
	if ((rp = getspent_r(spp,spbuf,splen)) == nullptr) {
	    rc = errno ;
	}
	return rc ;
}

#else /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */

int getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
	int	ec = EFAULT ;
	if (spp && spbuf) {
	    ec = EINVAL ;
	    memset(spp,0,sizeof(SPWD)) ;
	    if (splen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return -1 ;
}

#endif /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */

#if	defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0)

int getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
	CSPWD	*rp{} ;
	int	rc ;
	errno = 0 ;
	if ((rc = getspnam_r(n,spp,spbuf,splen,&rp)) >= 0) {
	    if (!rp) {
		errno = ENOENT :
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rc ;
}

#else /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */

int getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
	int	ec = EFAULT ;
	if (spp && spbuf && n) {
	    ec = EINVAL ;
	    memset(spp,0,sizeof(SPWD)) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return -1 ;
}

#endif /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */

#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */

void setspent() noex { }

void endspent() noex { }

SPWD *getspent() noex {
	errno = ENOENT ;
	return nullptr ;
}

SPWD *getspnam(cchar *) noex {
	errno = ENOENT ;
	return nullptr ;
}

int getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
	int	ec = EFAULT ;
	if (spp && spbuf) {
	    ec = EINVAL ;
	    memset(spp,0,sizeof(SPWD)) ;
	    if (splen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return -1 ;
}

int getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
	int	ec = EFAULT ;
	if (spp && spbuf && n) {
	    ec = EINVAL ;
	    memset(spp,0,sizeof(SPWD)) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return -1 ;
}

#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */


