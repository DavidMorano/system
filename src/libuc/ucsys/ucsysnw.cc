/* ucsysnw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for NETENT-DB access */
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
	getnwent_rp
	getnwnam_rp
	getnwnum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex
	unixret_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,
			cchar *) noex
	unixret_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,
				int t,uint32_t num) noex

	Arguments:
	nwp		NETENT pointer
	nwbuf		buffer pointer
	nwlen		buffer length
	n		name
	num		number
	t		type

	Returns:
	>=0	success
	<0	error (ERRNO is set on error)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucsysnw.h"

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

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getnwent_rp(NETENT *nwp,char *nwbuf,int prlen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    NETENT	*rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetent_r(nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = getresolvec(herr) ;
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
} /* end subroutine (getnwent_rp) */

#else /* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    CNETENT *rp ;
	    errno = 0 ;
	    if ((rp = getnetent_r(nwp,nwbuf,nwlen)) == nullptr) {
	        rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getnwent_rp) */

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */

unixret_t getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
	errno_t		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if (nwlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getnwent_rp) */

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR end */

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getnwnam_rp(NETENT *nwp,char *nwbuf,int prlen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    NETENT *rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetbyname_r(n,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = getresolvec(herr) ;
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
} /* end subroutine (getnwnam_rp) */

#else /* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    CNETENT *rp ;
	    errno = 0 ;
	    if ((rp = getnetbyname_r(n,nwp,nwbuf,nwlen)) == nullptr) {
		rc = -1 ;
	        void(rp) ;
	    }
        } else {
	    ec = EFAULT ;
	    rc = -1 ;
        }
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getnwnam_rp) */

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */

unixret_t getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
	errno_t		ec = EFAULT ;
	if (nwp && nwbuf && n) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if ((nwlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getnwnam_rp) */

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR ent */

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    NETENT *rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = getnetbyaddr_r(n,t,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	        if (rp == nullptr) {
		    ec = getresolvec(herr) ;
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
} /* end subroutine (getnwnum_rp) */

#else /* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (nwp && nwbuf) {
	    CNETENT *rp ;
	    errno = 0 ;
	    if ((rp = getnetbyaddr_r(n,t,nwp,nwbuf,nwlen)) == nullptr) {
	        rc = -1 ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getnwnum_rp) */

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */

/* NULL version */
unixret_t getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,int t,uint32_t) noex {
	errno_t		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memclear(nwp) ;
	    if ((nwlen > 0) && (t >= 0)) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getnwnum_t) */

#endif /* defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0) */
/* GETNWXXXR ent */


NETENT *getnwent() noex {
	return getnetent() ;
} /* end */

NETENT *getnwnam(cchar *n) noex {
	NETENT		*rp = nullptr ;
	errno_t		ec = 0 ;
	if (n) {
	    if (n[0]) {
		errno = 0 ;
		rp = getnetbyname(n) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getnwnam) */

NETENT *getnwnum(int t,uint32_t num) noex {
	return getnetbyaddr(num,t) ;
} /* end */


