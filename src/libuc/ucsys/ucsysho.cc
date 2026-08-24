/* ucsysho SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for HOSTENT-DB access */
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
	gethoent_rp
	gethonam_rp
	gethonum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex
	unixret_t gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *) noex
	unixret_t gethonum_rp(HOSTENT *hop,char *hobuf,int holen,
				uint32_t num,int t) noex

	Arguments:
	hop		HOSTENT pointer
	hobuf		buffer pointer
	holen		buffer length
	n		name
	num		number
	t		type

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucsysho.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	HO	HOSTENT


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

/* GETHOXXXR begin */
#if	defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)

#if	defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0)

/* GNU version (like on Linux) */
unixret_t gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    HOSTENT *rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = gethostent_r(hop,hobuf,holen,&rp,&herr)) == 0) {
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
} /* end subroutine (gethoent_rp) */

#else /* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    CHOSTENT *rp ;
	    errno = 0 ;
	    if ((rp = gethostent_r(hop,hobuf,holen)) == np) {
	        rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (gethoent_rp) */

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else /* for non-reentrant interfaces */

/* NULL version */
unixret_t gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
	errno_t		ec = EFAULT ;
	if (hop && hobuf) {
	    ec = EINVAL ;
	    memclear(hop) ;
	    if (holen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (gethoent_rp) */

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR end */

/* GETHOXXXR begin */
#if	defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)

#if	defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0)

/* GNU version (like on Linux) */
unixret_t gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    HOSTENT *rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    if ((ec = gethostbyname_r(n,hop,hobuf,holen,&rp,&herr)) == 0) {
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
} /* end subroutine (gethonam_rp) */

#else /* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    CHOSTENT *rp ;
	    errno = 0 ;
	    if ((rp = gethostbyname_r(n,hop,hobuf,holen)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (gethonam_rp) */

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */

/* NULL version */
unixret_t gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
	errno_t		ec = EFAULT ;
	if (hop && hobuf && n) {
	    ec = EINVAL ;
	    memclear(hop) ;
	    if ((holen > 0) && n[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (gethonam_rp) */

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR ent */

/* GETHOXXXR begin */
#if	defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)

#if	defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0)

/* GNU version (like on Linux) */
unixret_t gethonum_rp(HO *hop,char *hobuf,int holen,int af,cv *ap,int al) noex {
    	unixret_t	rc = 0 ;
    	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    HOSTENT *rp{} ;
	    int		herr{} ;
	    errno = 0 ;
	    cauto getbyaddr = gethostbyaddr_r ;
	    if ((ec = getbyaddr(ap,al,af,hop,hobuf,holen,&rp,&herr)) == 0) {
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
} /* end subroutine (gethonum_rp) */

#else /* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

/* POSIX draft-6 inspired version (like on SunOS) */
unixret_t gethoadd_rp(HO *hop,char *hobuf,int holen,int af,cv *ap,int al) noex {
    	cnullptr	np{} ;
	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (hop && hobuf) {
	    CHOSTENT *rp ;
	    errno = 0 ;
	    if ((rp = gethostbyaddr_r(ap,al,af,hop,hobuf,holen)) == np) {
		rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (gethoadd_rp) */

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */

/* NULL version */
unixret_t gethoadd_rp(HO *hop,char *hobuf,int holen,int af,cv *ap,int al) noex {
	errno_t		ec = EFAULT ;
	if (hop && hobuf && ap) {
	    ec = EINVAL ;
	    memclear(hop) ;
	    if ((holen > 0) && (al > 0) && (af >= 0)) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (gethoadd_rp) */

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR ent */


HOSTENT *gethoent() noex {
	return gethostent() ;
} /* end */

HOSTENT *gethonam(cchar *n) noex {
	HOSTENT		*rp = nullptr ;
	errno_t		ec = 0 ;
	if (n) {
	    if (n[0]) {
		errno = 0 ;
		rp = gethostbyname(n) ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (gethonam) */

HOSTENT *gethoadd(int af,cvoid *abuf,int alen) noex {
	HOSTENT		*rp = nullptr ;
	errno_t		ec = 0 ;
	if (abuf) {
	    if ((alen > 0) && (af >= 0)) {
		errno = 0 ;
		rp = gethostbyaddr(abuf,alen,af) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (gethoadd) */


