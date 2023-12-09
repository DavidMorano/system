/* ucgetpj */
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
	interface to access the 'project' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'project' database access subroutines.

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

#include	"ucsyspj.h"
#include	"ucgetpj.h"


/* local defines */

#if defined(SYSHAS_GETPJENTR) && (SYSHAS_GETPJENTR > 0)
#define	F_GETPJENTR	1
#else
#define	F_GETPJENTR	0
#endif
#if defined(SYSHAS_GETPJNAMR) && (SYSHAS_GETPJNAMR > 0)
#define	F_GETPJNAMR	1
#else
#define	F_GETPJNAMR	0
#endif
#if defined(SYSHAS_GETPJPIDR) && (SYSHAS_GETPJPIDR > 0)
#define	F_GETPJPIDR	1
#else
#define	F_GETPJPIDR	0
#endif
#if defined(SYSHAS_GETPJDEFR) && (SYSHAS_GETPJDEFR > 0)
#define	F_GETPJDEFR	1
#else
#define	F_GETPJDEFR	0
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetpj ;
    typedef int (ucgetpj::*mem_f)(ucentpj *,char *,int) noex ;
    struct ucgetpj {
	mem_f		m ;
	cchar		*name ;
	projid_t	pjid ;
	ucgetpj(cchar *n,projid_t u = 0) noex : name(n), pjid(u) { } ;
	int getpj_ent(ucentpj *,char *,int) noex ;
	int getpj_nam(ucentpj *,char *,int) noex ;
	int getpj_pid(ucentpj *,char *,int) noex ;
	int getpj_def(ucentpj *,char *,int) noex ;
	int operator () (ucentpj *,char *,int) noex ;
    } ; /* end struct (ucgetpj) */
}


/* forward references */


/* local variables */

constexpr bool f_sunos = F_SUNOS ;
constexpr bool f_darwin = F_DARWIN ;
constexpr bool f_linux = F_LINUX ;
constexpr bool f_getpjentr = F_GETPJENTR ;
constexpr bool f_getpjnamr = F_GETPJNAMR ;
constexpr bool f_getpjpidr = F_GETPJPIDR ;
constexpr bool f_getpjdefr = F_GETPJDEFR ;


/* exported subroutines */

int uc_getpjbegin() noex {
	errno = 0 ;
	setprojent() ;
	return (- errno) ;
}
/* end subroutine (uc_getpjbegin) */

int uc_getpjend() noex {
	errno = 0 ;
	endprojent() ;
	return (- errno) ;
}
/* end subroutine (uc_getpjend) */

int uc_getpjent(ucentpj *pjp,char *pjbuf,int pjlen) noex {
	ucgetpj		pjo(nullptr) ;
	pjo.m = &ucgetpj::getpj_ent ;
	return pjo(pjp,pjbuf,pjlen) ;
}
/* end subroutine (uc_getpjent) */

int uc_getpjnam(ucentpj *pjp,char *pjbuf,int pjlen,cchar *name) noex {
	ucgetpj		pjo(name) ;
	pjo.m = &ucgetpj::getpj_nam ;
	return pjo(pjp,pjbuf,pjlen) ;
}
/* end subroutine (uc_getpjnam) */

int uc_getpjpid(ucentpj *pjp,char *pjbuf,int pjlen,projid_t pjid) noex {
	ucgetpj		pjo(nullptr,pjid) ;
	pjo.m = &ucgetpj::getpj_pid ;
	return pjo(pjp,pjbuf,pjlen) ;
}
/* end subroutine (uc_getpjpid) */

int uc_getpjdef(ucentpj *pjp,char *pjbuf,int pjlen,cchar *name) noex {
	ucgetpj		pjo(name) ;
	pjo.m = &ucgetpj::getpj_def ;
	return pjo(pjp,pjbuf,pjlen) ;
}
/* end subroutine (uc_getpjnam) */


/* local subroutines */

static constexpr bool bit(uint v,int b) noex {
	return bool((v >> b) & 1) ;
}

int ucgetpj::operator () (ucentpj *pjp,char *pjbuf,int pjlen) noex {
	int		rs = SR_FAULT ;
	if (pjp && pjbuf) {
	    rs = SR_OVERFLOW ;
	    if (pjlen > 0) {
	        int	to_again = utimeout[uto_again] ;
	        int	to_nomem = utimeout[uto_nomem] ;
	        int	to_mfile = utimeout[uto_mfile] ;
	        int	to_nfile = utimeout[uto_nfile] ;
	        bool	f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(pjp,pjbuf,pjlen)) < 0) {
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
/* end subroutine (ucgetpj::operator) */

int ucgetpj::getpj_ent(ucentpj *pjp,char *pjbuf,int pjlen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_getpjentr) {
	    cint	ec = getpjent_rp(pjp,pjbuf,pjlen) ;
	    if (ec == 0) {
	        rs = pjp->size() ;
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
	    ucentpj	*rp = static_cast<ucentpj *>(getpjent()) ;
	    if (rp) {
	        rs = pjp->load(pjbuf,pjlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	return rs ;
}
/* end subroutine (ucgetpj::getpj_ent) */

int ucgetpj::getpj_nam(ucentpj *pjp,char *pjbuf,int pjlen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_getpjnamr) {
	        cint	ec = getpjnam_rp(pjp,pjbuf,pjlen,name) ;
	        if (ec == 0) {
	            rs = pjp->size() ;
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
	        ucentpj		*rp = static_cast<ucentpj *>(getpjnam(name)) ;
	        if (rp) {
	            rs = pjp->load(pjbuf,pjlen,rp) ;
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
/* end subroutine (ucgetpj::getpj_nam) */

int ucgetpj::getpj_pid(ucentpj *pjp,char *pjbuf,projid_t pjlen) noex {
	int		rs = SR_INVALID ;
	if (bit(pjid,31)) pjid = getprojid() ;
	if (pjid != projid_t(-1)) {
	    errno = 0 ;
	    if constexpr (f_getpjpidr) {
	        cint	ec = getpjpid_rp(pjp,pjbuf,pjlen,pjid) ;
	        if (ec == 0) {
	            rs = pjp->size() ;
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
	        ucentpj		*rp = static_cast<ucentpj *>(getpjpid(pjid)) ;
	        if (rp) {
	            rs = pjp->load(pjbuf,pjlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if-constexpr (selection) */
	    if constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (valid PID) */
	return rs ;
}
/* end subroutine (ucgetpj::getpj_pid) */

int ucgetpj::getpj_def(ucentpj *pjp,char *pjbuf,int pjlen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_getpjdefr) {
	        cint	ec = getpjdef_rp(pjp,pjbuf,pjlen,name) ;
	        if (ec == 0) {
	            rs = pjp->size() ;
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
	        ucentpj		*rp = static_cast<ucentpj *>(getpjdef(name)) ;
	        if (rp) {
	            rs = pjp->load(pjbuf,pjlen,rp) ;
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
/* end subroutine (ucgetpj::getpj_def) */


