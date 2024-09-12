/* ucgroup */
/* lang=C++20 */

/* additional operaring-system support for GROUP-DB access */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	I define some subroutines that are missing from some systems,
	who will remain nameless for now (Apple Darwin).

	Synopsis:
	int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex

	Synopsis:
	int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *) noex

	Synopsis:
	int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

*******************************************************************************/

#include	<envstandards.h>
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"ucgroup.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	GROUP	*rp ;
	int	rc = 0 ;
	errno = 0 ;
	if ((rp = getgrent_r(grp,grbuf,grlen)) == nullptr) {
	    rc = errno ;
	}
	void(rp) ;
	return rc ;
}

int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	GROUP	*rp ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrnam_r(n,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rc ;
}

int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	GROUP	*rp ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrgid_r(gid,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rc ;
}

#elif	 defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

int getgrent_rp(GROUP *,char *,int) noex {
	errno = ENOENT ;
	return -1 ;
}

int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	GROUP	*rp{} ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrnam_r(n,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rc ;
}

int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	GROUP	*rp{} ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrgid_r(gid,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rc ;
}

#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	GROUP	*rp{} ;
	int	rc = 0 ;
	errno = 0 ;
	if ((rc = getgrent_r(grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rs ;
}

int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	GROUP	*rp{} ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrnam_r(n,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = errno ;
	    }
	} else {
	    rc = errno ;
	}
	return rs ;
}

int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	CGROUP	*rp{} ;
	int	rc ;
	errno = 0 ;
	if ((rc = getgrgid_r(gid,grp,grbuf,grlen,&rp)) >= 0) {
	    if ((rc == 0) && (!rp)) {
		errno = ENOENT ;
		rc = (-errno) ;
	    }
	} else {
	    rc = (-errno) ;
	}
	return rs ;
}

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */


