/* ucsysho SUPPORT */
/* lang=C++20 */

/* additional operaring-system support for HOSTENT-DB access */
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
	gethoent_rp
	gethonam_rp
	gethonum_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex
	int gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *) noex
	int gethonum_rp(HOSTENT *hop,char *hobuf,int holen,
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
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clanguage.h>

#include	"ucsysho.h"
#include	"ucsys.h"


/* local defines */

#define	HO	HOSTENT


/* local namespaces */


/* local typedefs */


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
int getprent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
	HOSTENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = gethostent_r(hop,hobuf,holen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
	HOSTENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getnetent_r(hop,hobuf,holen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else

int gethoent_rp(HOSTENT *hop,char *hobuf,int holen) noex {
	int		ec = EFAULT ;
	if (hop && hobuf) {
	    ec = EINVAL ;
	    memset(hop,0,sizeof(HOSTENT)) ;
	    if (holen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR end */

/* GETHOXXXR begin */
#if	defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)

#if	defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0)

/* GNU version (like on Linux) */
int getprnam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
	HOSTENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = gethostbyname_r(n,hop,hobuf,holen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int gethonam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
	HOSTENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = gethostbyname_r(n,hop,hobuf,holen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else

int getwtnam_rp(HOSTENT *hop,char *hobuf,int holen,cchar *n) noex {
	int		ec = EFAULT ;
	if (hop && hobuf &&n) {
	    ec = EINVAL ;
	    memset(hop,0,sizeof(HOSTENT)) ;
	    if ((holen > 0) && n[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR ent */

/* GETHOXXXR begin */
#if	defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)

#if	defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0)

/* GNU version (like on Linux) */
int gethonum_rp(HO *hop,char *hobuf,int holen,cvoid *ap,int al,int af) noex {
	HOSTENT		*rp{} ;
	int		ec ;
	int		herr{} ;
	errno = 0 ;
	if ((ec = gethostbyaddr_r(ap,al,af,hop,hobuf,holen,&rp,&herr)) == 0) {
	    if (rp == nullptr) {
		ec = ucsys_getec(herr) ;
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

/* POSIX draft-6 inspired version (like on SunOS) */
int gethoad_rp(HO *hop,char *hobuf,int holen,cvoid *ap,int al,int af) noex {
	HOSTENT		*rp ;
	int		rc ;
	errno = 0 ;
	if ((rp = gethostbyaddr_r(ap,al,af,hop,hobuf,holen)) == nullptr) {
	    rc = errno ;
	}
	return rc ;
}

#endif	/* defined(SYSHAS_GETHOGNUR) && (SYSHAS_GETHOGNUR > 0) */

#else

int gethoadd_rp(HO *hop,char *hobuf,int holen,cvoid *ap,int al,int af) noex {
	int		ec = EFAULT ;
	if (hop && hobuf && ap) {
	    ec = EINVAL ;
	    memset(hop,0,sizeof(HOSTENT)) ;
	    if ((holen > 0) && (al > 0) && (af >= 0)) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0) */
/* GETHOXXXR ent */


HOSTENT *gethoent() noex {
	return gethostent() ;
}

HOSTENT *gethonam(cchar *n) noex {
	HOSTENT		*rp = nullptr ;
	errno = EFAULT ;
	if (n) {
	    errno = EINVAL ;
	    if (n[0]) {
		errno = 0 ;
		rp = gethostbyname(n) ;
	    }
	}
	return rp ;
}

HOSTENT *gethoadd(cvoid *abuf,int alen,int af) noex {
	HOSTENT		*rp = nullptr ;
	errno = EFAULT ;
	if (abuf) {
	    errno = EINVAL ;
	    if ((alen > 0) && (af >= 0)) {
		errno = 0 ;
		rp = gethostbyaddr(abuf,alen,af) ;
	    }
	}
	return rp ;
}


