/* ucsyspj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for PEOJECT-DB access */
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
	getpjent_rp
	getpjnam_rp
	getpjpid_rp
	getpjdef_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getpjent_rp(PROJECT *grp,char *pjbuf,int pjlen) noex
	unixret_t getpjnam_rp(PROJECT *grp,char *pjbuf,int pjlen,cchar *) noex
	unixret_t getpjpid_rp(PROJECT *grp,char *pjbuf,int pjlen,
			projid_t pid) noex
	unixret_t getpjdef_rp(PROJECT *grp,char *pjbuf,int pjlen,cchar *) noex

	Returns:
	>=0	success
	<0	error (ERRNO set on error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucsyspj.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)

#if	defined(SYSHAS_GETPJENTR) && (SYSHAS_GETPJENTR > 0)

unixret_t getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pjp && pjbuf) {
	    CPROJECT *rp ;
	    errno = 0 ;
	    if ((rp = getprojent(pjp,pjbuf,pjlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
        } else {
	    ec = EFAULT ;
	    rc = -1 ;
        }
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpjent_rp) */

#else /* defined(SYSHAS_GETPJENTR) && (SYSHAS_GETPJENTR > 0) */

unixret_t getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
        errno_t		ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memclear(pjp) ;
            if (pjlen > 0) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjent_rp) */

#endif /* defined(SYSHAS_GETPJENTR) && (SYSHAS_GETPJENTR > 0) */

#if	defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0)

unixret_t getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pjp && pjbuf && n) {
	    CPROJECT *rp ;
	    errno = 0 ;
	    if ((rp = getprojbyname(n,pjp,pjbuf,pjlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ex ;
	return rc ;
} /* end subroutine */

#else /* defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0) */

unixret_t getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memclear(pjp) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return ec ;
} /* end subroutine (getpjent_rp) */

#endif /* defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0) */

#if	defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0)

unixret_t getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t pid) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pjp && pjbuf) {
	    CPROJECT *rp ;
	    errno = 0 ;
	    if ((rp = getprojbyid(pid,pjp,pjbuf,pjlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpjpid_np) */

#else /* defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0) */

unixret_t getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memclear(pjp) ;
            if (pjlen > 0) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjpid_np) */

#endif /* defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0) */

#if	defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0)

unixret_t getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
    	cnullptr	np{} ;
	errno_t		ec = EFAULT ;
	if (pjp && pjbuf && n) {
	    CPROJECT *rp ;
	    errno = 0 ;
	    if ((rp = getdefaultproj(n,pjp,pjbuf,pjlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpjdef_rp) */

#else /* defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0) */

unixret_t getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memclear(pjp) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return ec ;
} /* end subroutine (getpjdef_rp) */

#endif /* defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0) */

#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
projid_t getprojid() noex {
	return projid_t(0) ;
} /* end */
#endif

void setprojent() noex { }

void endprojent() noex { }

PROJECT	*getpjent() noex {
	errno = ENOSYS ;
	return nullptr ;
} /* end */

PROJECT	*getpjnam(cchar *) noex {
	errno = ENOSYS ;
	return nullptr ;
} /* end */

PROJECT	*getpjpid(projid_t) noex {
	errno = ENOSYS ;
	return nullptr ;
} /* end */

PROJECT	*getpjdef(cchar *) noex {
	errno = ENOSYS ;
	return nullptr ;
} /* end */

unixret_t getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memclear(pjp) ;
            if (pjlen > 0) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjent_np) */

unixret_t getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memclear(pjp) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjnam_np) */

unixret_t getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t) noex {
        errno_t		ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memclear(pjp) ;
            if (pjlen > 0) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjpid_rp) */

unixret_t getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        errno_t     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memclear(pjp) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOSYS ;
            }
	} /* end if (non-null) */
        errno = ec ;
        return -1 ;
} /* end subroutine (getpjdef_np) */

#endif /*  defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


