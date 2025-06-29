/* ucsyspw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
	errno_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex
	errno_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *) noex
	errno_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex

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
#include	<localmisc.h>

#include	"ucsyspw.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

typedef const void	cv ;


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
errno_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    PASSWD	*rp{} ;
	    errno = 0 ;
	    if ((ec = getpwent_r(pwp,pwbuf,pwlen,&rp)) == 0) {
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

/* POSIX draft-version (like in SunOS) */
errno_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    PASSWD	*rp ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((rp = getpwent_r(pwp,pwbuf,pwlen)) == nullptr) {
	        ec = errno ;
	    }
	    void(rp) ;
	} else {
	    errno = ec ;
	}
	return ec ;
}

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
errno_t getpwent_rp(PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if (pwlen > 0) {
	        ec = ENOSYS ;
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
errno_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf && n) {
	    PASSWD	*rp{} ;
	    errno = 0 ;
	    if ((ec = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == 0) {
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

/* POSIX version (like in SunOS) */
errno_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int pwlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf && n) {
	    PASSWD	*rp{} ;
	    errno = 0 ;
	    if ((ec = getpwnam_r(n,pwp,pwbuf,pwlen,&rp)) == 0) {
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

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
errno_t getpwnam_rp(PASSWD *pwp,char *pwbuf,int,cchar *n) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf && n) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if ((pwlen > 0) && n[0]) {
	        ec = ENOSYS ;
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
errno_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    PASSWD	*rp{} ;
	    errno = 0 ;
	    if ((ec = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == 0) {
	        if (rp == nullptr) {
		    ec = ENOENT ;
		    errno = ec ;
	        }
	    } else if (ec > 0) {
		errno = ec ;
	    } else {
	        ex = EBUGCHECK ;
	        errno = ec ;
	    }
	} else {
	    errno = ec ;
	}
	return ec ;
}

#else

/* POSIX version (like in SunOS) */
errno_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    PASSWD	*rp{} ;
	    errno = 0 ;
	    if ((ec = getpwuid_r(uid,pwp,pwbuf,pwlen,&rp)) == 0) {
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

#endif /* defined(SYSHAS_GETPWGNUR) && (SYSHAS_GETPWGNUR > 0) */

#else

/* NULL version (like in Apple-Darwin) */
errno_t getpwuid_rp(PASSWD *pwp,char *pwbuf,int pwlen,uid_t) noex {
	int		ec = EFAULT ;
	if (pwp && pwbuf) {
	    ec = EINVAL ;
	    memclear(pwp) ;
	    if (pwlen > 0) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0) */
/* GETPWUIDR ent */


