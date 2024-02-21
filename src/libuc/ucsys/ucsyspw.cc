/* ucsyspw SUPPORT */
/* lang=C++20 */

/* additional operaring-system support for PASSWD-DB access */
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
	getpwent_rp
	getpwnam_rp
	getpwuid_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex
	int getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *) noex
	int getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clanguage.h>

#include	"ucsyspw.h"


/* local defines */


/* local namespaces */


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
int getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	PASSWD		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getpwent_r(pwp,pwbuf,pwlen,&rp)) == 0) {
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
int getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	PASSWD		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getpwent_r(pwp,pwbuf,pwlen)) == nullptr) {
	    ec = errno ;
	}
	void(rp) ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
int getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memset(pwp,0,sizeof(PASSWD)) ;
	    if (pwlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPWENTR) && (SYSHAS_GETPWENTR > 0) */
/* GETPWENTR end */

/* GETPWNAMR begin */
#if	defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0)

#if	defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0)

/* GNU version (like in Linux) */
int getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
	PASSWD		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == 0) {
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

/* POSIX version (like in SunOS) */
int getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
	PASSWD		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == 0) {
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

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
int getpwnam_rp(PASSWD *pwp,char *pwbuf,int,cchar *n) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf && n) {
	    ec = EINVAL ;
	    memset(pwp,0,sizeof(PASSWD)) ;
	    if ((pwlen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0) */
/* GETPWNAMR ent */

/* GETPWUIDR begin */
#if	defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0)

#if	defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0)

/* GNU version (like in Linux) */
int getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	PASSWD		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
		errno = ec ;
	    }
	} else if (ec < 0) {
	    ex = EBUGCHECK ;
	    errno = ec ;
	}
	return ec ;
}

#else

/* POSIX version (like in SunOS) */
int getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	PASSWD		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == 0) {
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

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
int getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memset(pwp,0,sizeof(PASSWD)) ;
	    if (pwlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0) */
/* GETPWUIDR ent */


