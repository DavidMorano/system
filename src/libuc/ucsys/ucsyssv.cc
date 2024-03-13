/* ucsyssv SUPPORT */
/* lang=C++20 */

/* additional operaring-system support for SERVENT-DB access */
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
	getsvent_rp
	getsvnam_rp
	getsvpor_rp

	Description:
	I define some subroutines that are missing from some systems,
	which will remain nameless for now (Apple Darwin).

	Synopsis:
	int getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex
	int getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cc*,cc *) noex
	int getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int,cc *) noex

	Arguments:
	svp		NETENT pointer
	svbuf		buffer pointer
	svlen		buffer length
	n		name
	num		port

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

#include	"ucsyssv.h"


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
int getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
	SERVENT		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getservent_r(svp,svbuf,svlen,&rp)) == 0) {
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
int getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
	SERVENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getservent_r(svp,svbuf,svlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getsvent_rp(SERVENT *svp,char *svbuf,int svlen) noex {
	int		ec = EFAULT ;
	if (svp && svbuf) {
	    ec = EINVAL ;
	    memset(svp,0,sizeof(SERVENT)) ;
	    if (svlen > 0) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */
/* GETSVXXXR end */

/* HETPRXXXR begin */
#if	defined(SYSHAS_HETPRXXXR) && (SYSHAS_HETPRXXXR > 0)

#if	defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0)

/* GNU version (like on Linux) */
int getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cchar *n,cchar *p) noex {
	SERVENT		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getservbyname_r(n,p,svp,svbuf,svlen,&rp)) == 0) {
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
int getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cchar *n,cchar *p) noex {
	SERVENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getservbyname_r(n,p,svp,svbuf,svlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getsvnam_rp(SERVENT *svp,char *svbuf,int svlen,cchar *n,cchar *p) noex {
	int		ec = EFAULT ;
	if (svp && svbuf && n && p) {
	    ec = EINVAL ;
	    memset(svp,0,sizeof(SERVENT)) ;
	    if ((svlen > 0) && n[0] && p[0]) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_HETPRXXXR) && (SYSHAS_HETPRXXXR > 0) */
/* HETPRXXXR ent */

/* GETSVXXXR begin */
#if	defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0)

#if	defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0)

/* GNU version (like on Linux) */
int getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cchar *p) noex {
	SERVENT		*rp{} ;
	int		ec ;
	errno = 0 ;
	if ((ec = getservbyport_r(num,p,svp,svbuf,svlen,&rp)) == 0) {
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
int getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cchar *p) noex {
	SERVENT		*rp ;
	int		ec = 0 ;
	errno = 0 ;
	if ((rp = getservbyport_r(num,p,svp,svbuf,svlen)) == nullptr) {
	    ec = errno ;
	}
	(void) rp ;
	return ec ;
}

#endif	/* defined(SYSHAS_GETSVGNUR) && (SYSHAS_GETSVGNUR > 0) */

#else

/* NULL version (like on Apple Darwin) */
int getsvpor_rp(SERVENT *svp,char *svbuf,int svlen,int num,cchar *p) noex {
	int		ec = EFAULT ;
	if (svp && svbuf && p) {
	    ec = EINVAL ;
	    memset(svp,0,sizeof(SERVENT)) ;
	    if ((svlen > 0) && (num >= 0)) {
	        ec = ENOENT ;
	    }
	}
	errno = ec ;
	return ec ;
}

#endif /* defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0) */
/* GETSVXXXR ent */


SERVENT *getsvent() noex {
	return getservent() ;
}

SERVENT *getsvnam(cchar *n,cchar *p) noex {
	SERVENT		*rp = nullptr ;
	errno = EFAULT ;
	if (n && p) {
	    errno = EINVAL ;
	    if (n[0] && p[0]) {
		errno = 0 ;
		rp = getservbyname(n,p) ;
	    }
	}
	return rp ;
}

SERVENT *getsvpor(int num,cchar *p) noex {
	SERVENT		*rp = nullptr ;
	errno = EFAULT ;
	if (p) {
	    errno = EINVAL ;
	    if (num >= 0) {
	        errno = 0 ;
	        rp = getservbyport(num,p) ;
	    }
	}
	return rp ;
}


