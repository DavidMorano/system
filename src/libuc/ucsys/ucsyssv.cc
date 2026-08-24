/* ucsyssv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* additional operaring-system support for SERVENT-DB access */
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
	getsvent_rp
	getsvnam_rp
	getsvpor_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	unixret_t getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex
	unixret_t getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cc*,cc *) noex
	unixret_t getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int,cc *) noex

	Arguments:
	svp		NETENT pointer
	svbuf		buffer pointer
	svlen		buffer length
	n		name
	num		port

	Returns:
	>=0	success
	<0	error (ERRNO set on error)

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

#include	"ucsyssv.h"

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

/* GETSVXXXR begin */
#if	defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0)

#if	defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    SERVENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getservent_r(svp,svbuf,svlen,&rp)) == 0) {
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
} /* end subroutine (getsvent_np) */

#else /* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

/* POSIX draft-6 inspired version (like on Solaris®) */
unixret_t getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    CSERVENT *rp ;
	    errno = 0 ;
	    if ((rp = getservent_r(svp,svbuf,svlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec  EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getsvent_np) */

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

else /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */

/* NULL version (like on Apple Darwin) */
unixret_t getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
	errno_t		ec = EFAULT ;
	if (svp && svbuf) {
	    ec = EINVAL ;
	    memclear(svp) ;
	    if (svlen > 0) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getsvent_np) */

#endif /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */
/* GETSVXXXR end */

/* HETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cc *n,cc *p) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    SERVENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getservbyname_r(n,p,svp,svbuf,svlen,&rp)) == 0) {
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
} /* end subroutine (getsvnam_np) */

#else /* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

/* POSIX draft-6 inspired version */
unixret_t getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cc *n,cc *p) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    CSERVENT *rp ;
	    errno = 0 ;
	    if ((rp = getservbyname_r(n,p,svp,svbuf,svlen)) == np) {
	        rc = -1 ;
		void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return ec ;
} /* end subroutine (getsvnam_np) */

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
unixret_t getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cc *n,cc *p) noex {
	errno_t		ec = EFAULT ;
	if (svp && svbuf && n && p) {
	    ec = EINVAL ;
	    memclear(svp) ;
	    if ((svlen > 0) && n[0] && p[0]) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getsvnam_np) */

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* HETPRXXXR ent */

/* GETSVXXXR begin */
#if	defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0)

#if	defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0)

/* GNU version (like on Linux) */
unixret_t getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cc *p) noex {
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    CSERVENT *rp{} ;
	    errno = 0 ;
	    if ((ec = getservbyport_r(num,p,svp,svbuf,svlen,&rp)) == 0) {
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
} /* end subroutine (getsvpor_np) */

#else /* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

/* POSIX draft-6 inspired version */
unixret_t getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cc *p) noex {
    	cnullptr	np{} ;
    	unixret_t	rc = 0 ;
	errno_t		ec = 0 ;
	if (svp && svbuf) {
	    CSERVENT *rp ;
	    errno = 0 ;
	    if ((rp = getservbyport_r(num,p,svp,svbuf,svlen)) == np) {
		rc = -1 ;
	        void(rp) ;
	    }
	} else {
	    ec = EFAULT ;
	    rc = -1 ;
	}
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getsvpor_np) */

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

#else /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */

/* NULL version (like on Apple Darwin) */
unixret_t getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cc *p) noex {
	errno_t		ec = EFAULT ;
	if (svp && svbuf && p) {
	    ec = EINVAL ;
	    memclear(svp) ;
	    if ((svlen > 0) && (num >= 0)) {
	        ec = ENOSYS ;
	    }
	} /* end if (non-null) */
	errno = ec ;
	return -1 ;
} /* end subroutine (getsvpor_np) */

#endif /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */
/* GETSVXXXR ent */


SERVENT *getsvent() noex {
	return getservent() ;
} /* end */

SERVENT *getsvnam(cchar *n,cchar *p) noex {
	SERVENT		*rp = nullptr ;
	errno_t		ec = 0 ;
	if (n && p) {
	    if (n[0] && p[0]) {
		errno = 0 ;
		rp = getservbyname(n,p) ;
	    } else {
	        ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getsvnam) */

SERVENT *getsvpor(int num,cchar *p) noex {
	SERVENT		*rp = nullptr ;
	errno_t		ec = 0 ;
	if (p) {
	    if (num >= 0) {
	        errno = 0 ;
	        rp = getservbyport(num,p) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getsvpor) */


