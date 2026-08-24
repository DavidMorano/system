/* ucgetpw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

	= 2017-05-17, David A-D- Morano
	I did some update to use C++11 ('if_constexpr').

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_getpw{x}

	Names:
	uc_getpwbegin
	uc_getpwent
	uc_getpwnam
	uc_getpwuid
	uc_getpwend

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

	Synopsis:
	int uc_getpwbegin() noex
	int uc_getpwent(ucentpw *,char *,int) noex
	int uc_getpwnam(ucentpw *,char *,int,cchar *) noex
	int uc_getpwuid(ucentpw *,char *,int,uid_t) noex
	int uc_getpwend() noex

	Note:
	1. Coding note: On some platforms, the non-reentrant versions
	of these subroutines do NOT set 'errno'.
	2. I made no special handling for those platforms that do
	not support reentrant variations for one or more of these
	subroutines. I otherwise I could have put a mutex lock
	around everything to get the same behavior as on other
	(minimally sane) platforms. I will not name names (messed-up
	Darwin) so as not to offend any particular operation system.
	But, YES, curse those operating systems that did not have
	the minimal decency to put mutex locks around all of these
	subroutines within their standard-C library.  In a parting
	thought: curse Apple Darwin.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIC® */
#include	<sys/param.h>		/* POSIC® */
#include	<unistd.h>		/* POSIC® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |SYSDBPW(3u)| */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<ucsyspw.h>		/* LIBUC */

#include	"ucgetpw.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETPWENTR) && (SYSHAS_GETPWENTR > 0)
#define	F_GETPWENTR	1
#else
#define	F_GETPWENTR	0
#endif
#if defined(SYSHAS_GETPWNAMR) && (SYSHAS_GETPWNAMR > 0)
#define	F_GETPWNAMR	1
#else
#define	F_GETPWNAMR	0
#endif
#if defined(SYSHAS_GETPWUIDR) && (SYSHAS_GETPWUIDR > 0)
#define	F_GETPWUIDR	1
#else
#define	F_GETPWUIDR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetpw ;
    typedef int (ucgetpw::*mem_m)(ucentpw *,char *,int) noex ;
    struct ucgetpw {
	mem_m	m ;
	cchar	*name ;
	uid_t	uid ;
	ucgetpw(cchar *n,uid_t u = 0) noex : name(n), uid(u) { } ;
	int getpw_ent(ucentpw *,char *,int) noex ;
	int getpw_nam(ucentpw *,char *,int) noex ;
	int getpw_uid(ucentpw *,char *,int) noex ;
	int operator () (ucentpw *,char *,int) noex ;
    } ; /* end struct (ucgetpw) */
} /* end namespace */


/* forward references */

constexpr bool bit(uint v,int b) noex {
	return bool((v >> b) & 1) ;
} /* end */


/* local variables */

constexpr uid_t		uidend		= uid_t(-1) ;
constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;
constexpr bool		f_getpwentr	= F_GETPWENTR ;
constexpr bool		f_getpwnamr	= F_GETPWNAMR ;
constexpr bool		f_getpwuidr	= F_GETPWUIDR ;


/* exported variables */


/* exported subroutines */

int uc_getpwbegin() noex {
	errno = 0 ;
	setpwent() ;
	return (neg errno) ;
} /* end subroutine (uc_getpwbegin) */

int uc_getpwend() noex {
	errno = 0 ;
	endpwent() ;
	return (neg errno) ;
} /* end subroutine (uc_getpwend) */

int uc_getpwent(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	ucgetpw		pwo(nullptr) ;
	pwo.m = &ucgetpw::getpw_ent ;
	return pwo(pwp,pwbuf,pwlen) ;
} /* end subroutine (uc_getpwent) */

int uc_getpwnam(ucentpw *pwp,char *pwbuf,int pwlen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		ucgetpw		pwo(name) ;
		pwo.m = &ucgetpw::getpw_nam ;
		rs = pwo(pwp,pwbuf,pwlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getpwnam) */

int uc_getpwuid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
    	int		rs = SR_INVALID ;
	if (bit(uid,31)) {
	    uid = getuid() ;
	}
	if (uid != uidend) ylikely {
	    ucgetpw	pwo(nullptr,uid) ;
	    pwo.m = &ucgetpw::getpw_uid ;
	    rs = pwo(pwp,pwbuf,pwlen) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getpwuid) */


/* local subroutines */

int ucgetpw::operator () (ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	if (pwp && pwbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; pwlen > 0) ylikely {
	        repeat {
	            if ((rs = (this->*m)(pwp,pwbuf,pwlen)) < 0) {
			rs = err(rs) ;
		    }
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucgetpw::operator) */

int ucgetpw::getpw_ent(ucentpw *pwp,char *pwbuf,int pwlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_NOSYS ;
	if_constexpr (f_getpwentr) {
	    if ((rs = getpwent_rp(pwp,pwbuf,pwlen)) >= 0) {
	        rs = pwp->size() ;
	    } else {
		rs = (neg errno) ;
	    }
	} else {
	    SYSDBPW	*ep = getpwent() ;
	    if (ucentpw *rp = cast_static<ucentpw *>(ep) ; rp != np) {
	        rs = pwp->load(pwbuf,pwlen,rp) ;
	    } else {
	        rs = (neg errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
} /* end subroutine (ucgetpw::getpw_ent) */

int ucgetpw::getpw_nam(ucentpw *pwp,char *pwbuf,int pwlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_getpwnamr) {
            if ((rs = getpwnam_rp(pwp,pwbuf,pwlen,name)) >= 0) {
                rs = pwp->size() ;
            } else {
                rs = (neg errno) ;
            }
        } else {
            SYSDBPW         *ep = getpwnam(name) ;
            if (ucentpw *rp = cast_static<ucentpw *>(ep) ; rp != np) {
                rs = pwp->load(pwbuf,pwlen,rp) ;
            } else {
                rs = (neg errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
} /* end subroutine (ucgetpw::getpw_nam) */

int ucgetpw::getpw_uid(ucentpw *pwp,char *pwbuf,int pwlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_getpwuidr) {
            if ((rs = getpwuid_rp(pwp,pwbuf,pwlen,uid)) >= 0) {
                rs = pwp->size() ;
	    } else {
                rs = (neg errno) ;
            }
        } else {
            SYSDBPW         *ep = getpwuid(uid) ;
            if (ucentpw *rp = cast_static<ucentpw *>(ep) ; rp != np) {
                rs = pwp->load(pwbuf,pwlen,rp) ;
            } else {
                rs = (neg errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
} /* end subroutine (ucgetpw::getpw_uid) */


