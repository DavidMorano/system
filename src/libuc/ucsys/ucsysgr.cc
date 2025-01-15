/* ucsysgr SUPPORT */
/* encoding=ISO8859-1 */
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
	which systems will remain nameless for now (Apple Darwin).

	Synopsis:
	errno_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex
	errno_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *) noex
	errno_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<memx.h>

#include	"ucsysgr.h"


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
errno_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
        int             ec  = EFAULT ;
	if (grp && grbuf) {
            GROUP	*rp{} ;
            errno = 0 ;
	    ec = 0 ;
            if ((ec = getgrent_r(grp,grbuf,grlen,&rp)) == 0) {
                if (rp == nullptr) {
                    ec = ENOENT ;
                    errno = ec ;
                }
            } else if (ec > 0) {
                errno = ec ;
	    } else {
                ec = EBUGCHECK ;
                errno = ec ;
            }
	} else {
	    errno = ec ;
	} /* end if (non-null) */
        return ec ;
}

#else

/* POSIX draft-version (like in SunOS) */
errnot_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    GROUP	*rp ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((rp = getgrent_r(grp,grbuf,grlen)) == nullptr) {
	        ec = errno ;
	    }
	    void(rp) ;
	} else {
	    errno = ec ;
	}
	return ec ;
}

#endif /* defined(SYSHAS_GETGRGNUR) && (SYSHAS_GETGRGNUR > 0) */

#else

errno_t getgrent_rp(GROUP *grp,char *grbuf,int grlen) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if (grlen > 0) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0) */
/* GETGRENTR end */

/* GETGRNAMR begin */
#if	defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0)

errno_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (grp && grbuf && n) {
	    GROUP	*rp{} ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((ec = getgrnam_r(n,grp,grbuf,grlen,&rp)) == 0) {
	        if (rp == nullptr) {
		    ec = ENOENT ;
		    errno = ec ;
	        }
	    } else if (ec > 0) {
		errno = ec ;
	    } else {
	        ec = EBUGCHECK ;
	        errno = ec ;
	    }
	} else {
	    errno = ec ;
	}
	return ec ;
}

#else

errno_t getgrnam_rp(GROUP *grp,char *grbuf,int grlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (grp && grbuf && n) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if ((grlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0) */
/* GETGRNAMR ent */

/* GETGRGIDR begin */
#if	defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0)

errno_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    GROUP	*rp{} ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((ec = getgrgid_r(gid,grp,grbuf,grlen,&rp)) == 0) {
	        if (rp == nullptr) {
		    ec = ENOENT ;
		    errno = ec ;
	        }
	    } else if (ec > 0) {
	        errno = ec ;
	    } else {
	        ec = EBUGCHECK ;
	        errno = ec ;
	    }
	} else {
	    errno = ec ;
	}
	return ec ;
}

#else

errno_t getgrgid_rp(GROUP *grp,char *grbuf,int grlen,gid_t gid) noex {
	int		ec = EFAULT ;
	if (grp && grbuf) {
	    ec = EINVAL ;
	    memclear(grp) ;
	    if (grlen > 0) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0) */
/* GETGRGIDR ent */


