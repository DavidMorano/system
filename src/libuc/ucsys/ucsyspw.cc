/* ucsyspw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for PASSWD-DB access */
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
	getpwent_rp
	getpwnam_rp
	getpwuid_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex
	unixret_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *) noex
	unixret_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex

	Returns:
	>=0	success
	<0	error (ERRNO set on error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"ucsyspw.h"

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

/* GETPWENTR begin */
#if	defined(SYSHAS_GETPWENTR) && (SYSHAS_GETPWENTR > 0)

#if	defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0)

/* GNU version (like in Linux) */
unixret_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pwp && pwbuf) {
	    PASSWD *rp{} ;
	    errno = 0 ;
	    if ((ec = getpwent_r(pwp,pwbuf,pwlen,&rp)) == 0) {
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
} /* end subroutine (getpwent_rp) */

#else /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

/* POSIX draft-version (like in SunOS) */
unixret_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	int		ec = 0 ;
	if (pwp && pwbuf) {
	    errno = 0 ;
	    if (CPASSWD *rp ; (rp = getpwent_r(pwp,pwbuf,pwlen)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpwent_rp) */

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else /* defined(SYSHAS_GETPWENTR) && (SYSHAS_GETPWENTR > 0) */

/* NULL version (like in Apple-Darwin) */
unixret_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	errno_t		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if (pwlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getpwent_rp) */

#endif /* defined(SYSHAS_GETPWENTR) && (SYSHAS_GETPWENTR > 0) */
/* GETPWENTR end */

/* GETPWNAMR begin */
#if	defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0)

#if	defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0)

/* GNU version (like in Linux) */
unixret_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pwp && pwbuf && n) {
	    PASSWD *rp{} ;
	    errno = 0 ;
	    if ((ec = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == 0) {
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
} /* end subroutine (getpwnam_rp) */

#else /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

/* POSIX version (like in SunOS) */
unixret_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pwp && pwbuf && n) {
	    CPASSWD *rp{} ;
	    errno = 0 ;
	    if ((rp = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpwnam_rp) */

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else /* defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0) */

/* NULL version (like in Apple-Darwin) */
errno_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int,cchar *n) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf && n) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if ((pwlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getpwnam_rp) */

#endif /* defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0) */
/* GETPWNAMR ent */

/* GETPWUIDR begin */
#if	defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0)

#if	defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0)

/* GNU version (like in Linux) */
unixret_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pwp && pwbuf) {
	    PASSWD *rp{} ;
	    errno = 0 ;
	    if ((ec = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == 0) {
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
} /* end subroutine (getpwuid_rp) */

#else /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

/* POSIX version (like in SunOS) */
unixret_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (pwp && pwbuf) {
	    CPASSWD *rp{} ;
	    errno = 0 ;
	    if ((rp = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getpwuid_rp) */

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else /* defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0) */

/* NULL version (like in Apple-Darwin) */
unixret_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t) noex {
	errno_t		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if (pwlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getpwuid_rp) */

#endif /* defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0) */
/* GETPWUIDR ent */


