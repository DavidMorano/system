/* ucsysnw SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* additional operaring-system support for NETENT-DB access */
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
	getnwent_rp
	getnwnam_rp
	getnwnum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	errno_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex
	errno_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *) noex
	errno_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,
				int t,uint32_t num) noex

	Arguments:
	nwp		NETENT pointer
	nwbuf		buffer pointer
	nwlen		buffer length
	n		name
	num		number
	t		type

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

#include	"ucsysnw.h"
#include	"ucsys.h"


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

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
errno_t getnwent_rp(NETENT *nwp,char *nwbuf,int prlen) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetent_r(nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = ucsys_getresolvec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
errno_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((rp = getnetent_r(nwp,nwbuf,nwlen)) == nullptr) {
	        ec = errno ;
	    }
	    (void) rp ;
	} else {
	    errno = ec ;
	}
	return ec ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else

errno_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if (nwlen > 0) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR end */

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
errno_t getnwnam_rp(NETENT *nwp,char *nwbuf,int prlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetbyname_r(n,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = ucsys_getresolvec(herr) ;
	            errno = ec ;
	        }
	    } else if (ec > 0) {
	        ec = errno ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
errno_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((rp = getnetbyname_r(n,nwp,nwbuf,nwlen)) == nullptr) {
	        ec = errno ;
	    }
	    (void) rp ;
        } else {
	    errno = ec ;
        }
	return ec ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else

errno_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf && n) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if ((nwlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR ent */

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
errno_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetbyaddr_r(n,t,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = ucsys_getresolvec(herr) ;
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

#else /* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
errno_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    NETENT	*rp ;
	    errno = 0 ;
	    ec = 0 ;
	    if ((rp = getnetbyaddr_r(n,t,nwp,nwbuf,nwlen)) == nullptr) {
	        rc = errno ;
	    }
	} else {
	    errno = ec ;
	}
	return rc ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */

errno_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if ((nwlen > 0) && (t >= 0)) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR ent */


NETENT *getnwent() noex {
	return getnetent() ;
}

NETENT *getnwnam(cchar *n) noex {
	NETENT		*rp = nullptr ;
	errno = EFAULT ;
	if (n) {
	    errno = EINVAL ;
	    if (n[0]) {
		errno = 0 ;
		rp = getnetbyname(n) ;
	    }
	}
	return rp ;
}

NETENT *getnwnum(int t,uint32_t num) noex {
	return getnetbyaddr(num,t) ;
}


