/* ucsyspj SUPPORT */
/* lang=C++20 */

/* additional operaring-system support for PEOJECT-DB access */
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
	getpjent_rp
	getpjnam_rp
	getpjpid_rp
	getpjdef_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int getpjent_rp(PROJECT *grp,char *pjbuf,int pjlen) noex
	int getpjnam_rp(PROJECT *grp,char *pjbuf,int pjlen,cchar *) noex
	int getpjpid_rp(PROJECT *grp,char *pjbuf,int pjlen,projid_t pid) noex
	int getpjdef_rp(PROJECT *grp,char *pjbuf,int pjlen,cchar *) noex

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>

#include	"ucsyspj.h"


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

int getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
	PROJECT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprojent(pjp,pjbuf,pjlen)) == nullptr) {
	    ec = errno ;
	}
	void(rp) ;
	return ec ;
}

#else

int getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if (pjlen > 0) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

#endif /* defined(SYSHAS_GETPJENTR) && (SYSHAS_GETPJENTR > 0) */

#if	defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0)

int getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
	PROJECT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprojbyname(n,pjp,pjbuf,pjlen)) == nullptr) {
	    ec = errno ;
	}
	void(rp) ;
	return ec ;
}

#else

int getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

#endif /* defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0) */

#if	defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0)

int getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t pid) noex {
	PROJECT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprojbyid(pid,pjp,pjbuf,pjlen)) == nullptr) {
	    ec = errno ;
	}
	void(rp) ;
	return ec ;
}

#else

int getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if (pjlen > 0) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

#endif /* defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0) */

#if	defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0)

int getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
	PROJECT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getdefaultproj(n,pjp,pjbuf,pjlen)) == nullptr) {
	    ec = errno ;
	}
	void(rp) ;
	return ec ;
}

#else

int getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

#endif /* defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0) */

#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
projid_t getprojid() noex  {
	return projid_t(0) ;
}
#endif

void setprojent() noex { }

void endprojent() noex { }

PROJECT	*getpjent() noex {
	errno = ENOENT ;
	return nullptr ;
}
PROJECT	*getpjnam(cchar *) noex {
	errno = ENOENT ;
	return nullptr ;
}
PROJECT	*getpjpid(projid_t) noex {
	errno = ENOENT ;
	return nullptr ;
}
PROJECT	*getpjdef(cchar *) noex {
	errno = ENOENT ;
	return nullptr ;
}

int getpjent_rp(PROJECT *pjp,char *pjbuf,int pjlen) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if (pjlen > 0) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

int getpjnam_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

int getpjpid_rp(PROJECT *pjp,char *pjbuf,int pjlen,projid_t) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if (pjlen > 0) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

int getpjdef_rp(PROJECT *pjp,char *pjbuf,int pjlen,cchar *n) noex {
        int     	ec = EFAULT ;
        if (pjp && pjbuf && n) {
            ec = EINVAL ;
            memset(pjp,0,sizeof(PROJECT)) ;
            if ((pjlen > 0) && n[0]) {
                ec = ENOENT ;
            }
        }
        errno = ec ;
        return ec ;
}

#endif /*  defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


