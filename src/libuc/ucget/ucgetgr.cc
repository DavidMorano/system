/* ucgetgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
	uc_getgr{x}

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'group' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'group' database access subroutines.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>
#include	<ucsysgr.h>

#include	"ucgetgr.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETGRENTR) && (SYSHAS_GETGRENTR > 0)
#define	F_GETGRENTR	1
#else
#define	F_GETGRENTR	0
#endif
#if defined(SYSHAS_GETGRNAMR) && (SYSHAS_GETGRNAMR > 0)
#define	F_GETGRNAMR	1
#else
#define	F_GETGRNAMR	0
#endif
#if defined(SYSHAS_GETGRGIDR) && (SYSHAS_GETGRGIDR > 0)
#define	F_GETGRGIDR	1
#else
#define	F_GETGRGIDR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetgr ;
    typedef int (ucgetgr::*mem_m)(ucentgr *,char *,int) noex ;
    struct ucgetgr {
	mem_m	m ;
	cchar	*name ;
	gid_t	gid ;
	ucgetgr(cchar *n,gid_t u = 0) noex : name(n), gid(u) { } ;
	int getgr_ent(ucentgr *,char *,int) noex ;
	int getgr_nam(ucentgr *,char *,int) noex ;
	int getgr_gid(ucentgr *,char *,int) noex ;
	int operator () (ucentgr *,char *,int) noex ;
    } ; /* end struct (ucgetgr) */
}


/* forward references */

static constexpr bool bit(uint v,int b) noex {
	return bool((v >> b) & 1) ;
}


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;
constexpr bool		f_getgrentr = F_GETGRENTR ;
constexpr bool		f_getgrnamr = F_GETGRNAMR ;
constexpr bool		f_getgrgidr = F_GETGRGIDR ;

constexpr gid_t		gidend = gid_t(-1) ;


/* exported variables */


/* exported subroutines */

int uc_getgrbegin() noex {
	errno = 0 ;
	setgrent() ;
	return (- errno) ;
}
/* end subroutine (uc_getgrbegin) */

int uc_getgrend() noex {
	errno = 0 ;
	endgrent() ;
	return (- errno) ;
}
/* end subroutine (uc_getgrend) */

int uc_getgrent(ucentgr *grp,char *grbuf,int grlen) noex {
	ucgetgr		gro(nullptr) ;
	gro.m = &ucgetgr::getgr_ent ;
	return gro(grp,grbuf,grlen) ;
}
/* end subroutine (uc_getgrent) */

int uc_getgrnam(ucentgr *grp,char *grbuf,int grlen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		ucgetgr		gro(name) ;
		gro.m = &ucgetgr::getgr_nam ;
		rs = gro(grp,grbuf,grlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getgrnam) */

int uc_getgrgid(ucentgr *grp,char *grbuf,int grlen,gid_t gid) noex {
    	int		rs = SR_OK ;
	if (bit(gid,31)) {
	    gid = getgid() ;
	}
	if (gid != gidend) {
	    ucgetgr	gro(nullptr,gid) ;
	    gro.m = &ucgetgr::getgr_gid ;
	    rs = gro(grp,grbuf,grlen) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getgrgid) */


/* local subroutines */

int ucgetgr::operator () (ucentgr *grp,char *grbuf,int grlen) noex {
	int		rs = SR_FAULT ;
	if (grp && grbuf) {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; grlen > 0) {
	        repeat {
	            if ((rs = (this->*m)(grp,grbuf,grlen)) < 0) {
			rs = err(rs) ;
		    }
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetgr::operator) */

int ucgetgr::getgr_ent(ucentgr *grp,char *grbuf,int grlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	errno = 0 ;
	if_constexpr (f_getgrentr) {
	    cint	ec = getgrent_rp(grp,grbuf,grlen) ;
	    if (ec == 0) {
	        rs = grp->size() ;
	    } else if (ec > 0) {
	        rs = (-ec) ;
	    } else {
		if (errno) {
		    rs = (-errno) ;
		} else {
		    rs = SR_IO ;
		}
	    }
	} else {
	    SYSDBGR	*ep = getgrent() ;
	    if (ucentgr *rp = cast_static<ucentgr *>(ep) ; rp != np) {
	        rs = grp->load(grbuf,grlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetgr::getgr_ent) */

int ucgetgr::getgr_nam(ucentgr *grp,char *grbuf,int grlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_getgrnamr) {
            cint    ec = getgrnam_rp(grp,grbuf,grlen,name) ;
            if (ec == 0) {
                rs = grp->size() ;
            } else if (ec > 0) {
                rs = (-ec) ;
            } else {
                if (errno) {
                    rs = (-errno) ;
                } else {
                    rs = SR_IO ;
                }
            }
        } else {
            SYSDBGR         *ep = getgrnam(name) ;
            if (ucentgr *rp = cast_static<ucentgr *>(ep) ; rp != np) {
                rs = grp->load(grbuf,grlen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetgr::getgr_nam) */

int ucgetgr::getgr_gid(ucentgr *grp,char *grbuf,int grlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_getgrgidr) {
            cint    ec = getgrgid_rp(grp,grbuf,grlen,gid) ;
            if (ec == 0) {
                rs = grp->size() ;
            } else if (ec > 0) {
                rs = (-ec) ;
            } else {
                if (errno) {
                    rs = (-errno) ;
                } else {
                    rs = SR_IO ;
                }
            }
        } else {
            SYSDBGR         *ep = getgrgid(gid) ;
            if (ucentgr *rp = cast_static<ucentgr *>(ep) ; rp != np) {
                rs = grp->load(grbuf,grlen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetgr::getgr_gid) */


