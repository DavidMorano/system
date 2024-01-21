/* ucsysgr SUPPORT */
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

	Names:
	getgrent_rp
	getgrnam_rp
	getgrgid_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex
	int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *) noex
	int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

*******************************************************************************/

#include	<envstandards.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clanguage.h>

#include	"ucsysgr.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* GETGRENTR begin */
#if	defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0)

#if     defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0)

/* GNU version (like in Linux) */
int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
        GROUP          *rp{} ;
        int             ec ;
        errno = 0 ;
        if ((ec = getgrent_r(grp,grbuf,grlen,&rp)) == 0) {
            if (rp == nullptr) {
                ec = ENOENT ;
                errno = ec ;
            }
        } else if (ec < 0) {
            ec = EBUGCHECK ;
            errno = ec ;
        }
        return ec ;
}

#else

/* POSIX draft-version (like in SunOS) */
int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	GROUP		*rp ;
	int		rc = 0 ;
	errno = 0 ;
	if ((rp = getgrent_r(grp,grbuf,grlen)) == nullptr) {
	    rc = errno ;
	}
	void(rp) ;
	return rc ;
}

#endif /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

#else

int getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memset(grp,0,sizeof(GROUP)) ;
	    if (grlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0) */
/* GETGRENTR end */

/* GETGRNAMR begin */
#if	defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0)

int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	GROUP		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getgrnam_r(n,grp,grbuf,grlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
		errno = ec ;
	    }
	} else {
	    ec = EBUGCHECK ;
	    errno = ec ;
	}
	return ec ;
}

#else

int getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (grp && grbuf && n) {
	    ec = EINVAL ;
	    memset(grp,0,sizeof(GROUP)) ;
	    if ((grlen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0) */
/* GETGRNAMR ent */

/* GETGRGIDR begin */
#if	defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0)

int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	GROUP		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getgrgid_r(gid,grp,grbuf,grlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
		errno = ec ;
	    }
	} else if (ec < 0) {
	    ec = EBUGCHECK ;
	    errno = ec ;
	
	}
	return ec ;
}

#else

int getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memset(grp,0,sizeof(GROUP)) ;
	    if (grlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0) */
/* GETGRGIDR ent */


