/* ucgetsp SUPPORT */
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
	uc_getsp{x}

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'spwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'spwd' database access subroutines.

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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSYD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<spwd.h>		/* LIBUC */

#include	"ucgetsp.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETSPENTR) && (SYSHAS_GETSPENTR > 0)
#define	F_GETSPENTR	1
#else
#define	F_GETSPENTR	0
#endif
#if defined(SYSHAS_GETSPNAMR) && (SYSHAS_GETSPNAMR > 0)
#define	F_GETSPNAMR	1
#else
#define	F_GETSPNAMR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* type */
using gnu::setspent ;			/* subroutine */
using gnu::endspent ;			/* subroutine */
using gnu::getspent ;			/* subroutine */
using gnu::getspnam ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetsp ;
    typedef int (ucgetsp::*mem_m)(ucentsp *,char *,int) noex ;
    struct ucgetsp {
	mem_m		m ;
	cchar		*name ;
	ucgetsp(cchar *n = nullptr) noex : name(n) { } ;
	int getsp_ent(ucentsp *,char *,int) noex ;
	int getsp_nam(ucentsp *,char *,int) noex ;
	int operator () (ucentsp *,char *,int) noex ;
    } ; /* end struct (ucgetsp) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr bool		f_sunos 	= F_SUNOS ;
constexpr bool		f_darwin 	= F_DARWIN ;
constexpr bool		f_linux 	= F_LINUX ;
constexpr bool		f_getspentr 	= F_GETSPENTR ;
constexpr bool		f_getspnamr 	= F_GETSPNAMR ;


/* exported variables */


/* exported subroutines */

int uc_getspbegin() noex {
	errno = 0 ;
	setspent() ;
	return (neg errno) ;
} /* end subroutine (uc_getspbegin) */

int uc_getspend() noex {
	errno = 0 ;
	endspent() ;
	return (neg errno) ;
} /* end subroutine (uc_getspend) */

int uc_getspent(ucentsp *spp,char *spbuf,int splen) noex {
	ucgetsp		spo(nullptr) ;
	spo.m = &ucgetsp::getsp_ent ;
	return spo(spp,spbuf,splen) ;
} /* end subroutine (uc_getspent) */

int uc_getspnam(ucentsp *spp,char *spbuf,int splen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		ucgetsp		spo(name) ;
		spo.m = &ucgetsp::getsp_nam ;
		rs = spo(spp,spbuf,splen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getspnam) */


/* local subroutines */

int ucgetsp::operator () (ucentsp *spp,char *spbuf,int splen) noex {
	int		rs = SR_FAULT ;
	if (spp && spbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; splen > 0) ylikely {
		repeat {
	            if ((rs = (this->*m)(spp,spbuf,splen)) < 0) {
			rs = err(rs) ;
	            } /* end if (error) */
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucgetsp::operator) */

int ucgetsp::getsp_ent(ucentsp *spp,char *spbuf,int splen) noex {
    	cnullptr	np{} ;
	int		rs ;
	if_constexpr (f_getspentr) {
	    if ((rs = getspent_rp(spp,spbuf,splen)) >= 0) {
	        rs = spp->size() ;
	    } else {
		rs = (neg errno) ;
	    }
	} else {
	    SYSDBSP	*ep = getspent() ;
	    if (ucentsp *rp = cast_static<ucentsp *>(ep) ; ep != np) {
	        rs = spp->load(spbuf,splen,rp) ;
	    } else {
	        rs = (neg errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
} /* end subroutine (ucgetsp::getsp_ent) */

int ucgetsp::getsp_nam(ucentsp *spp,char *spbuf,int splen) noex {
    	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_getspnamr) {
            if ((rs = getspnam_rp(spp,spbuf,splen,name)) >= 0) {
                rs = spp->size() ;
	    } else {
                rs = (neg errno) ;
            }
        } else {
            SYSDBSP         *ep = getspnam(name) ;
            if (ucentsp *rp = cast_static<ucentsp *>(ep) ; ep != np) {
                rs = spp->load(spbuf,splen,rp) ;
            } else {
                rs = (neg errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
} /* end subroutine (ucgetsp::getsp_nam) */


