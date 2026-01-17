/* ucsysgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for GROUP-DB access */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David A-D- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	getgrent_rp
	getgrnam_rp
	getgrgid_rp

	Description:
	I define some subroutines that are missing from some systems,
	which systems will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex
	unixret_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *) noex
	unixret_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex

	Returns:
	>=0	success
	<0	error (ERRNO set on error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"ucsysgr.h"

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

/* GETGRENTR begin */
#if	defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0)

#if     defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0)

/* GNU version (like in Linux) */
unixret_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
    	unixret_t	rc = 0 ;
        errno_t		ec  = 0 ;
	if (grp && grbuf) {
            GROUP *rp{} ;
            errno = 0 ;
            if ((ec = getgrent_r(grp,grbuf,grlen,&rp)) == 0) {
                if (rp == nullptr) {
                    ec = ENOENT ;
                    rc = -1 ;
                }
            } else if (ec > 0) {
                rc = -1 ;
	    } else {
                ec = EBUGCHECK ;
		rc = -1 ;
            }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
        return rc ;
} /* end subroutine (getgrent_rp) */

#else /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

/* POSIX draft-version (like in SunOS) */
unixret_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (grp && grbuf) {
	    CGROUP *rp ;
	    errno = 0 ;
	    if ((rp = getgrent_r(grp,grbuf,grlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getgrent_rp) */

#endif /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

#else /* defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0) */

/* NULL version */
unixret_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	errno_t		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if (grlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getgrent_rp) */

#endif /* defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0) */
/* GETGRENTR end */

/* GETGRNAMR begin */
#if	defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0)

#if     defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0)

unixret_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (grp && grbuf && n) {
	    GROUP *rp{} ;
	    errno = 0 ;
	    if ((ec = getgrnam_r(n,grp,grbuf,grlen,&rp)) == 0) {
	        if (rp == nullptr) {
		    ec = ENOENT ;
		    rc = -1 ;
	        }
	    } else if (ec > 0) {
		rc = -1 ;
	    } else {
	        ec = EBUGCHECK ;
	        rc = -1 ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getgrnam_rp) */

#else /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

/* Solaris version */
unixret_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (grp && grbuf && n) {
	    CGROUP *rp{} ;
	    errno = 0 ;
	    if ((rp = getgrnam_r(n,grp,grbuf,grlen,&rp)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getgrnam_rp) */

#endif /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

#else /* defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0) */

/* NULL version */
unixret_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	errno_t		ec = EFAULT ;
	if (grp && grbuf && n) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if ((grlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getgrnam_rp) */

#endif /* defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0) */
/* GETGRNAMR ent */

/* GETGRGIDR begin */
#if	defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0)

#if     defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0)

unixret_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (grp && grbuf) {
	    GROUP *rp{} ;
	    errno = 0 ;
	    if ((ec = getgrgid_r(gid,grp,grbuf,grlen,&rp)) == 0) {
	        if (rp == nullptr) {
		    ec = ENOENT ;
		    rc = -1 ;
	        }
	    } else if (ec > 0) {
	        rc = -1 ;
	    } else {
	        ec = EBUGCHECK ;
	        rc = -1 ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getgrgid_rp) */

#else /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

/* Solaris version */
unixret_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (grp && grbuf) {
	    CGROUP *rp{} ;
	    errno = 0 ;
	    if ((rp = getgrgid_r(gid,grp,grbuf,grlen,&rp)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getgrgid_rp) */

#endif /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

#else /* defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0) */

/* NULL version */
unixret_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if (grlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getgrgid_rp) */

#endif /* defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0) */
/* GETGRGIDR ent */


