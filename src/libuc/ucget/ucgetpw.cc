/* ucgetpw */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

	= 2017-05-17, David A-D- Morano
	I did some update to use C++11 ('if constexpr').

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

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
#include	<stdlib.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>
#include	<usysflag.h>

#include	"ucsyspw.h"
#include	"ucgetpw.h"


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


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetpw ;
    typedef int (ucgetpw::*mem_f)(ucentpw *,char *,int) noex ;
    struct ucgetpw {
	mem_f	m ;
	cchar	*name ;
	uid_t	uid ;
	ucgetpw(cchar *n,uid_t u = 0) noex : name(n), uid(u) { } ;
	int getpw_ent(ucentpw *,char *,int) noex ;
	int getpw_nam(ucentpw *,char *,int) noex ;
	int getpw_uid(ucentpw *,char *,int) noex ;
	int operator () (ucentpw *,char *,int) noex ;
    } ; /* end struct (ucgetpw) */
}


/* forward references */


/* local variables */

constexpr bool f_sunos = F_SUNOS ;
constexpr bool f_darwin = F_DARWIN ;
constexpr bool f_linux = F_LINUX ;
constexpr bool f_getpwentr = F_GETPWENTR ;
constexpr bool f_getpwnamr = F_GETPWNAMR ;
constexpr bool f_getpwuidr = F_GETPWUIDR ;


/* exported subroutines */

int uc_getpwbegin() noex {
	errno = 0 ;
	setpwent() ;
	return (- errno) ;
}
/* end subroutine (uc_getpwbegin) */

int uc_getpwend() noex {
	errno = 0 ;
	endpwent() ;
	return (- errno) ;
}
/* end subroutine (uc_getpwend) */

int uc_getpwent(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	ucgetpw		pwo(nullptr) ;
	pwo.m = &ucgetpw::getpw_ent ;
	return pwo(pwp,pwbuf,pwlen) ;
}
/* end subroutine (uc_getpwent) */

int uc_getpwnam(ucentpw *pwp,char *pwbuf,int pwlen,cchar *name) noex {
	ucgetpw		pwo(name) ;
	pwo.m = &ucgetpw::getpw_nam ;
	return pwo(pwp,pwbuf,pwlen) ;
}
/* end subroutine (uc_getpwnam) */

int uc_getpwuid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	ucgetpw		pwo(nullptr,uid) ;
	pwo.m = &ucgetpw::getpw_uid ;
	return pwo(pwp,pwbuf,pwlen) ;
}
/* end subroutine (uc_getpwuid) */


/* local subroutines */

static constexpr bool bit(uint v,int b) noex {
	return bool((v >> b) & 1) ;
}

int ucgetpw::operator () (ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	if (pwp && pwbuf) {
	    rs = SR_OVERFLOW ;
	    if (pwlen > 0) {
	        int	to_again = utimeout[uto_again] ;
	        int	to_nomem = utimeout[uto_nomem] ;
	        int	to_mfile = utimeout[uto_mfile] ;
	        int	to_nfile = utimeout[uto_nfile] ;
	        bool	f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(pwp,pwbuf,pwlen)) < 0) {
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
/* end subroutine (ucgetpw::operator) */

int ucgetpw::getpw_ent(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_getpwentr) {
	    cint	ec = getpwent_rp(pwp,pwbuf,pwlen) ;
	    if (ec == 0) {
	        rs = pwp->size() ;
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
	    ucentpw	*rp = static_cast<ucentpw *>(getpwent()) ;
	    if (rp) {
	        rs = pwp->load(pwbuf,pwlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	if constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetpw::getpw_ent) */

int ucgetpw::getpw_nam(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_getpwnamr) {
	        cint	ec = getpwnam_rp(pwp,pwbuf,pwlen,name) ;
	        if (ec == 0) {
	            rs = pwp->size() ;
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
	        ucentpw		*rp = static_cast<ucentpw *>(getpwnam(name)) ;
	        if (rp) {
	            rs = pwp->load(pwbuf,pwlen,rp) ;
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
/* end subroutine (ucgetpw::getpw_nam) */

int ucgetpw::getpw_uid(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_INVALID ;
	if (bit(uid,31)) uid = getuid() ;
	if (uid != uid_t(-1)) {
	    errno = 0 ;
	    if constexpr (f_getpwuidr) {
	        cint	ec = getpwuid_rp(pwp,pwbuf,pwlen,uid) ;
	        if (ec == 0) {
	            rs = pwp->size() ;
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
	        ucentpw		*rp = static_cast<ucentpw *>(getpwuid(uid)) ;
	        if (rp) {
	            rs = pwp->load(pwbuf,pwlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if-constexpr (selection) */
	    if constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (valid UID) */
	return rs ;
}
/* end subroutine (ucgetpw::getpw_uid) */


