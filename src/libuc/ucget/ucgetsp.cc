/* ucgetsp */
/* lang=C++20 */

/* UNIX� C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This module was originally written.

	= 2017-05-17, David A-D- Morano
	I did some update to use C++11 ('if constexpr').

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'spwd' database on all UNIX�
	platforms.  This code module provides a platform independent
	implementation of UNIX� 'spwd' database access subroutines.

	Note:
	1. Coding note: On some platforms, the non-reentrant versions
	of these subroutines do NOT set 'errno'.
	2. I made no special handling for those platforms that do
	not support reentrant variations for one or more of these
	subroutines. I otherwise I could have put a mutex lock
	around everything to get the same behavior as on other
	(minimally sane) platforms. I will not name names (f*ck-sh*t
	Darwin) so as not to offend any particular operation system.
	But, YES, f*ck those operating systems that did not have
	the minimal decency to put mutex locks around all of these
	subroutines within their standard-C library.  In a parting
	thought: f*ck Apple Darwin.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<errno.h>
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>
#include	<usysflag.h>

#include	"spwd.h"
#include	"ucgetsp.h"


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


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetsp ;
    typedef int (ucgetsp::*mem_f)(ucentsp *,char *,int) noex ;
    struct ucgetsp {
	mem_f	m ;
	cchar	*name ;
	ucgetsp(cchar *n = nullptr) noex : name(n) { } ;
	int getsp_ent(ucentsp *,char *,int) noex ;
	int getsp_nam(ucentsp *,char *,int) noex ;
	int operator () (ucentsp *,char *,int) noex ;
    } ; /* end struct (ucgetsp) */
}


/* forward references */


/* local variables */

constexpr bool f_sunos = F_SUNOS ;
constexpr bool f_darwin = F_DARWIN ;
constexpr bool f_linux = F_LINUX ;
constexpr bool f_getspentr = F_GETSPENTR ;
constexpr bool f_getspnamr = F_GETSPNAMR ;


/* exported subroutines */

int uc_getspbegin() noex {
	errno = 0 ;
	setspent() ;
	return (- errno) ;
}
/* end subroutine (uc_getspbegin) */

int uc_getspend() noex {
	errno = 0 ;
	endspent() ;
	return (- errno) ;
}
/* end subroutine (uc_getspend) */

int uc_getspent(ucentsp *spp,char *spbuf,int splen) noex {
	ucgetsp		spo(nullptr) ;
	spo.m = &ucgetsp::getsp_ent ;
	return spo(spp,spbuf,splen) ;
}
/* end subroutine (uc_getspent) */

int uc_getspnam(ucentsp *spp,char *spbuf,int splen,cchar *name) noex {
	ucgetsp		spo(name) ;
	spo.m = &ucgetsp::getsp_nam ;
	return spo(spp,spbuf,splen) ;
}
/* end subroutine (uc_getspnam) */


/* local subroutines */

int ucgetsp::operator () (ucentsp *spp,char *spbuf,int splen) noex {
	int		rs = SR_FAULT ;
	if (spp && spbuf) {
	    rs = SR_OVERFLOW ;
	    if (splen > 0) {
	        int	to_again = utimeout[uto_again] ;
	        int	to_nomem = utimeout[uto_nomem] ;
	        int	to_mfile = utimeout[uto_mfile] ;
	        int	to_nfile = utimeout[uto_nfile] ;
	        bool	f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(spp,spbuf,splen)) < 0) {
	                switch (rs) {
	                case SR_AGAIN:
		            if (to_again-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
		            break ;
	                case SR_NOMEM:
		            if (to_nomem-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
			    break ;
	                case SR_MFILE:
		            if (to_mfile-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
			    break ;
	                case SR_NFILE:
		            if (to_nfile-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
			    break ;
	                case SR_INTR:
	                    break ;
	                default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetsp::operator) */

int ucgetsp::getsp_ent(ucentsp *spp,char *spbuf,int splen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_getspentr) {
	    cint	ec = getspent_rp(spp,spbuf,splen) ;
	    if (ec == 0) {
	        rs = spp->size() ;
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
	    ucentsp	*rp = static_cast<ucentsp *>(getspent()) ;
	    if (rp) {
	        rs = spp->load(spbuf,splen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	if constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetsp::getsp_ent) */

int ucgetsp::getsp_nam(ucentsp *spp,char *spbuf,int splen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_getspnamr) {
	        cint	ec = getspnam_rp(spp,spbuf,splen,name) ;
	        if (ec == 0) {
	            rs = spp->size() ;
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
	        ucentsp		*rp = static_cast<ucentsp *>(getspnam(name)) ;
	        if (rp) {
	            rs = spp->load(spbuf,splen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if-constexpr (selection) */
	    if constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetsp::getsp_nam) */


