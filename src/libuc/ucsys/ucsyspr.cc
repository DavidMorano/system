/* ucsyspr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for PROTOENT-DB access */
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
	getprent_rp
	getprnam_rp
	getprnum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex
	unixret_t getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *) noex
	unixret_t getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex

	Arguments:
	prp		NETENT pointer
	prbuf		buffer pointer
	prlen		buffer length
	n		name
	num		number

	Returns:
	>=0	success
	<0	error (ERRNO is set on error)

	Notes:
	This coding below is really b*llsh*t tedious, but at least I
	only have to code this crap once here.  The tragedy is that
	I have coded this same crap in multiple places over the last
	couple of decades already.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucsyspr.h"

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

/* GETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (prp && prbuf) {
	    PROTOENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getprotoent_r(prp,prbuf,prlen,&rp)) == 0) {
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
} /* end subroutine (getprent_rp) */

#else /* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

/* POSIX draft-6 inspired version (like on Solaris®) */
unixret_t getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (prp && prbuf) {
	    CPROTOENT *rp ;
	    errno = 0 ;
	    if ((rp = getprotoent_r(prp,prbuf,prlen)) == np) {
	        rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getprent_rp) */

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */

/* NULL version (like on Apple Darwin) */
unixret_t getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
	errno_t		ec = EFAULT ;
	if (prp && prbuf) {
	    ec = EINVAL ;
	    memclear(prp) ;
	    if (prlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getprent_rp) */

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* GETPRXXXR end */

/* HETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (prp && prbuf && n) {
	    PROTOENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getprotobyname_r(n,prp,prbuf,prlen,&rp)) == 0) {
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
} /* end subroutine (getprnam_np) */

#else /* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

/* POSIX draft-6 inspired version */
unixret_t getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (prp && prbuf && n) {
	    CPROTOENT *rp ;
	    errno = 0 ;
	    if ((rp = getprotobyname_r(n,prp,prbuf,prlen)) == np) {
		rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getprnam_np) */

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */

/* NULL version (like on Apple Darwin) */
unixret_t getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
	errno_t		ec = EFAULT ;
	if (prp && prbuf && n) {
	    ec = EINVAL ;
	    memclear(prp) ;
	    if ((prlen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getprnam_np) */

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* HETPRXXXR ent */

/* GETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
	int		ec = EFAULT ;
	if (prp && prbuf) {
	    PROTOENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getprotobynumber_r(num,prp,prbuf,prlen,&rp)) == 0) {
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
} /* end subroutine (getprnum_rp) */

#else /* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

/* POSIX draft-6 inspired version */
unixret_t getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (prp && prbuf) {
	    CPROTOENT *rp ;
	    errno = 0 ;
	    if ((rp = getprotobynumber_r(num,prp,prbuf,prlen)) == np) {
		rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getprnum_rp) */

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
unixret_t getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
	errno_t		ec = EFAULT ;
	if (prp && prbuf) {
	    ec = EINVAL ;
	    memclear(prp) ;
	    if ((prlen > 0) && (num >= 0)) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getprnum_rp) */

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* GETPRXXXR ent */


PROTOENT *getprent() noex {
	return getprotoent() ;
} /* end */

PROTOENT *getprnam(cchar *n) noex {
	PROTOENT	*rp = nullptr ;
	errno_t		ec = 0 ;
	if (n) {
	    if (n[0]) {
		errno = 0 ;
		rp = getprotobyname(n) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getprnam) */

PROTOENT *getprnum(int num) noex {
	PROTOENT	*rp = nullptr ;
	errno_t		ec = 0 ;
	if (num >= 0) {
	    errno = 0 ;
	    rp = getprotobynumber(num) ;
	} else {
	    ec = EINVAL ;
	}
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getprnum) */


