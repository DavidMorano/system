/* ucsysnw SUPPORT */
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
	int getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex
	int getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *) noex
	int getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,
				uint32_t num,int t) noex

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

#include	<envstandards.h>
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clanguage.h>

#include	"ucsysnw.h"
#include	"ucsys.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* GETNWXXXR begin */
#if	defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)

#if	defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0)

/* GNU version (like on Linux) */
int getprent_rp(NETENT *nwp,char *nwbuf,int prlen) noex {
	NETENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = getnetent_r(nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
	NETENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getnetent_r(nwp,nwbuf,nwlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else

int getnwent_rp(NETENT *nwp,char *nwbuf,int nwlen) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memset(nwp,0,sizeof(NETENT)) ;
	    if (nwlen > 0) {
	        ec = ENOENT ;
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
int getprnam_rp(NETENT *nwp,char *nwbuf,int prlen,cchar *n) noex {
	NETENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = getnetbyname_r(n,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int getnwnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
	NETENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getnetbyname_r(n,nwp,nwbuf,nwlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else

int getwtnam_rp(NETENT *nwp,char *nwbuf,int nwlen,cchar *n) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf && n) {
	    ec = EINVAL ;
	    memset(nwp,0,sizeof(NETENT)) ;
	    if ((nwlen > 0) && n[0]) {
	        ec = ENOENT ;
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
int getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,uint32_t n,int t) noex {
	NETENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = getnetbyaddr_r(n,t,nwp,nwbuf,prlen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,uint32_t n,int t) noex {
	NETENT		*rp ;
	int		rc ;
	errno = 0 ;
	if ((rp = getnetbyaddr_r(n,t,nwp,nwbuf,nwlen)) == nullptr) {
	    rc = errno ;
	}
	return rc ;
}

#endif	/* defined(SYSHAS_GETNWGNUR) && (SYSHAS_GETNWGNUR > 0) */

#else

int getnwnum_rp(NETENT *nwp,char *nwbuf,int nwlen,uint32_t,int t) noex {
	int		ec = EFAULT ;
	if (nwp && nwbuf) {
	    ec = EINVAL ;
	    memset(nwp,0,sizeof(NETENT)) ;
	    if ((nwlen > 0) && (t >= 0)) {
	        ec = ENOENT ;
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

NETENT *getnwnum(uint32_t num,int t) noex {
	return getnetbyaddr(num,t) ;
}


