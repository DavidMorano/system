/* ucsyspr SUPPORT */
/* lang=C++20 */

/* additional operaring-system support for PROTOENT-DB access */
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
	getprent_rp
	getprnam_rp
	getprnum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex
	int getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *) noex
	int getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex

	Arguments:
	prp		NETENT pointer
	prbuf		buffer pointer
	prlen		buffer length
	n		name
	num		number

	Returns:
	0	success
	>0	errno
	<0	*should not happen*

	Notes:
	This coding below is really b*llsh*t tedious, but at least I
	only have to code this crap once here.  The tragedy is that
	I have coded this same crap in multiple places over the last
	couple of decades already.

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

#include	"ucsyspr.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* GETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
int getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
	PROTOENT	*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getprotoent_r(prp,prbuf,prlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
	        errno = ec ;
	    }
	} else if (ec > 0) {
	    ec = errno ;
	} else {
	    ec = BGCHECK ;
	    errno = ec ;
	}
	return ec ;
}

#else

/* POSIX draft-6 inspired version */
int getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
	PROTOENT	*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprotoent_r(prp,prbuf,prlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getprent_rp(PROTOENT *prp,char *prbuf,int prlen) noex {
	int		ec = EFAULT ;
	if (prp && prbuf) {
	    ec = EINVAL ;
	    memset(prp,0,sizeof(PROTOENT)) ;
	    if (prlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* GETPRXXXR end */

/* HETPRXXXR begin */
#if	defined(SYSHAS_HETPRXXXR) && (SYSHAS_HETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
int getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
	PROTOENT	*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getprotobyname_r(n,prp,prbuf,prlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
	        errno = ec ;
	    }
	} else if (ec > 0) {
	    ec = errno ;
	} else {
	    ec = EBUGCHECK ;
	    errno = ec ;
	}
	return ec ;
}

#else

/* POSIX draft-6 inspired version */
int getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
	PROTOENT	*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprotobyname_r(n,prp,prbuf,prlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getprnam_rp(PROTOENT *prp,char *prbuf,int prlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (prp && prbuf && n) {
	    ec = EINVAL ;
	    memset(prp,0,sizeof(PROTOENT)) ;
	    if ((prlen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_HETPRXXXR) && (SYSHAS_HETPRXXXR > 0) */
/* HETPRXXXR ent */

/* GETPRXXXR begin */
#if	defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)

#if	defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0)

/* GNU version (like on Linux) */
int getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
	PROTOENT	*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getprotobynumber_r(num,prp,prbuf,prlen,&rp)) == 0) {
	    if (rp == nullptr) {
		ec = ENOENT ;
	        errno = ec ;
	    }
	} else if (ec > 0) {
	    ec = errno ;
	} else {
	    ec = EBUGCHECK ;
	    errno = ec ;
	}
	return ec ;
}

#else

/* POSIX draft-6 inspired version */
int getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
	PROTOENT	*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getprotobynumber_r(num,prp,prbuf,prlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETPRGNUR) && (SYSHAS_GETPRGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getprnum_rp(PROTOENT *prp,char *prbuf,int prlen,int num) noex {
	int		ec = EFAULT ;
	if (prp && prbuf) {
	    ec = EINVAL ;
	    memset(prp,0,sizeof(PROTOENT)) ;
	    if ((prlen > 0) && (num >= 0)) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0) */
/* GETPRXXXR ent */


PROTOENT *getprent() noex {
	return getprotoent() ;
}

PROTOENT *getprnam(cchar *n) noex {
	PROTOENT	*rp = nullptr ;
	errno = EFAULT ;
	if (n) {
	    errno = EINVAL ;
	    if (n[0]) {
		errno = 0 ;
		rp = getprotobyname(n) ;
	    }
	}
	return rp ;
}

PROTOENT *getprnum(int num) noex {
	PROTOENT	*rp = nullptr ;
	errno = EINVAL ;
	if (num >= 0) {
	    errno = 0 ;
	    rp = getprotobynumber(num) ;
	}
	return rp ;
}


