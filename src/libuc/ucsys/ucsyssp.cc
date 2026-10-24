/* ucsyssp SUPPORT */
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
	getspent_rp
	getspnam_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getspent_rp(SPWD *,char *,int) noex
	unixret_t getspnam_rp(SPWD *,char *,int,cchar *) noex

	Returns:
	>=0	success
	<0	error (ERRNO set on error)

	Notes:
	1. The whole "shadow" password concept was created by
	Julianne (Julie) Frances Haugh II (in 1987 originally for
	SCO Xenix®).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucsyssp.h"

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

#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)

#if	defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0)

#if	defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0)

/* GUN version */
unixret_t getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
	unixret_t	ru = unixretgood ;
	errno_t		ec = EOK ;
	if (spp && spbuf) {
	    SPWD *rp{} ;
	    errno = 0 ;
	    if ((ec = getspent_r(spp,spbuf,splen,&rp)) == 0) {
		if (! rp) {
		    ec = ENOENT ;
		    ru = unixretbad ;
		}
	    } else if (ec > 0) {
	        rc = uniretbad ;
	    } else {
	        ec = EBUGCHECK ;
	        ru = unixretbad ;
	    }
	} else {
	    ec = EFAULT ;
	    ru = unixretbad ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return ru ;
} /* end subroutine (getspent_rp) */

#else /* defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0) */

/* Solaris version */
unixret_t getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
    	cnullptr	np{} ;
	unixret_t	ru = unixretgood ;
	errno_t		ec = EOK ;
	if (spp && spbuf) {
	    errno = 0 ;
	    if (CSPWD *rp{} ; (rp = getspent_r(spp,spbuf,splen)) == np) {
	        ru = unixretbad ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    ru = unixretbad ;
	}
	if (ec) errno = ec ;
	return ru ;
} /* end subroutine (getspent_rp) */

#endif /* defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0) */

#else /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */

unixret_t getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
    	errno_t		ec = EFAULT ;
	if (spp && spbuf) {
	    ec = EINVAL ;
	    memclear(spp) ;
	    if (splen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getspent_rp) */

#endif /* defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0) */

#if	defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0)

#if	defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0)

/* GUN version */
unixret_t getspnam_rp(SPWD *spp,char *spbuf,int splen) noex {
	unixret_t	ru = unixretgood ;
	errno_t		ec = EOK ;
	if (spp && spbuf) {
	    SPWD *rp{} ;
	    errno = 0 ;
	    if ((ec = getspent_r(spp,spbuf,splen,&rp)) == 0) {
		if (! rp) {
		    ec = ENOENT ;
		    ru = unixretbad ;
		}
	    } else if (ec > 0) {
	        ru = unixretbad ;
	    } else {
	        ec = EBUGCHECK ;
	        ru = unixretbad ;
	    }
	} else {
	    ec = EFAULT ;
	    ru = unixretbad ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return ru ;
} /* end subroutine (getspnam_rp) */

#else /* defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0) */

/* Solaris version */
unixret_t getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
    	cnullptr	np{} ;
	unixret_t	ru = unixretgood ;
	errno_t		ec = EOK ;
	if (spp && spbuf && n) {
	    CSPWD *rp{} ;
	    errno = 0 ;
	    if ((rp = getspnam_r(n,spp,spbuf,splen)) == np) {
		ru = unixretbad ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    ru = unixretbad ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return ru ;
} /* end subroutine (getspnam_rp) */

#endif /* defined(SYSHAS_GETSPGNUR) && (SYSHAS_GETSPGNUR > 0) */

#else /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */

/* BULL version */
unixret_t getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
	int		ec = EFAULT ;
	if (spp && spbuf && n) {
	    ec = EINVAL ;
	    memclear(spp) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
} /* end subroutine (getspnam_rp) */

#endif /* defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0) */

#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */

/* NULL version */
unixret_t getspent_rp(SPWD *spp,char *spbuf,int splen) noex {
	errno_t		ec = EFAULT ;
	if (spp && spbuf) {
	    ec = EINVAL ;
	    memclear(spp) ;
	    if (splen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getspent_rp) */

/* NULL version */
unixret_t getspnam_rp(SPWD *spp,char *spbuf,int splen,cchar *n) noex {
	errno_t		ec = EFAULT ;
	if (spp && spbuf && n) {
	    ec = EINVAL ;
	    memclear(spp) ;
	    if ((splen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getspnam_rp) */

#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */


