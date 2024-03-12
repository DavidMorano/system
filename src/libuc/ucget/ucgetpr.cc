/* ucgetpr SUPPORT */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getprnam

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'protocol' (by name).

	Synopsis:
	int uc_getprnam(ucentpr *pep,char *pebuf,int pelen,cchar *name) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- pebuf		proto-ent buffer pointer
	- pelen		proto-ent buffer length
	- name		protocol name to lookup

	Returns:
	0		entry was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_OVERFLOW	buffer overflow


	Name:
	uc_getprnum

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'protocol' (by number).

	Synopsis:
	int uc_getprnum(ucentpr *pep,char *pebuf,int pelen,int p) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- rbuf		user supplied buffer to hold result
	- rlen		length of user supplied buffer
	- proto		service to lookup

	Returns:
	0		entry was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_OVERFLOW	buffer overflow


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>
#include	<usysflag.h>

#include	"ucsyspr.h"
#include	"ucgetpr.h"


/* local defines */

#if defined(SYSHAS_GETPRXXXR) && (SYSHAS_GETPRXXXR > 0)
#define	F_GETPRXXXR	1
#else
#define	F_GETPRXXXR	0
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetpr ;
    typedef int (ucgetpr::*mem_f)(ucentpr *,char *,int) noex ;
    struct ucgetpr {
	mem_f		m ;
	cchar		*name ;
	int		num ;
	ucgetpr(cchar *n,int u = 0) noex : name(n), num(u) { } ;
	int getpr_ent(ucentpr *,char *,int) noex ;
	int getpr_nam(ucentpr *,char *,int) noex ;
	int getpr_num(ucentpr *,char *,int) noex ;
	int operator () (ucentpr *,char *,int) noex ;
    } ; /* end struct (ucgetpr) */
}


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;

constexpr bool		f_getprxxxr = F_GETPRXXXR ;


/* exported variables */


/* exported subroutines */

int uc_getprbegin(int stayopen) noex {
	errno = 0 ;
	setprotoent(stayopen) ;
	return (- errno) ;
}
/* end subroutine (uc_getprbegin) */

int uc_getprend() noex {
	errno = 0 ;
	endprotoent() ;
	return (- errno) ;
}
/* end subroutine (uc_getprend) */

int uc_getprent(ucentpr *prp,char *prbuf,int prlen) noex {
	ucgetpr		pro(nullptr) ;
	pro.m = &ucgetpr::getpr_ent ;
	return pro(prp,prbuf,prlen) ;
}
/* end subroutine (uc_getprent) */

int uc_getprnam(ucentpr *prp,char *prbuf,int prlen,cchar *name) noex {
	ucgetpr		pro(name) ;
	pro.m = &ucgetpr::getpr_nam ;
	return pro(prp,prbuf,prlen) ;
}
/* end subroutine (uc_getprnam) */

int uc_getprnum(ucentpr *prp,char *prbuf,int prlen,int num) noex {
	ucgetpr		pro(nullptr,num) ;
	pro.m = &ucgetpr::getpr_num ;
	return pro(prp,prbuf,prlen) ;
}
/* end subroutine (uc_getprnum) */


/* local subroutines */

int ucgetpr::operator () (ucentpr *prp,char *prbuf,int prlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (prp && prbuf) {
	    rs = SR_OVERFLOW ;
	    if (prlen > 0) {
	        timecount	to_again(utimeout[uto_again]) ;
	        timecount	to_nomem(utimeout[uto_nomem]) ;
	        timecount	to_mfile(utimeout[uto_mfile]) ;
	        timecount	to_nfile(utimeout[uto_nfile]) ;
		bool		f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(prp,prbuf,prlen)) < 0) {
	                switch (rs) {
	                case SR_AGAIN:
			    if ((rs1 = to_again) < 0) {
				if (rs1 != SR_TIMEDOUT) rs = rs1 ;
				f_exit = true ;
			    }
		            break ;
	                case SR_NOMEM:
			    if ((rs1 = to_nomem) < 0) {
				if (rs1 != SR_TIMEDOUT) rs = rs1 ;
				f_exit = true ;
			    }
		            break ;
	                case SR_MFILE:
			    if ((rs1 = to_mfile) < 0) {
				if (rs1 != SR_TIMEDOUT) rs = rs1 ;
				f_exit = true ;
			    }
			    break ;
	                case SR_NFILE:
			    if ((rs1 = to_nfile) < 0) {
				if (rs1 != SR_TIMEDOUT) rs = rs1 ;
				f_exit = true ;
			    }
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
/* end subroutine (ucgetpr::operator) */

int ucgetpr::getpr_ent(ucentpr *prp,char *prbuf,int prlen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_getprxxxr) {
	    cint	ec = getprent_rp(prp,prbuf,prlen) ;
	    if (ec == 0) {
	        rs = prp->size() ;
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
	    ucentpr	*rp = static_cast<ucentpr *>(getprent()) ;
	    if (rp) {
	        rs = prp->load(prbuf,prlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	if constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetpr::getpr_ent) */

int ucgetpr::getpr_nam(ucentpr *prp,char *prbuf,int prlen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_getprxxxr) {
	        cint	ec = getprnam_rp(prp,prbuf,prlen,name) ;
	        if (ec == 0) {
	            rs = prp->size() ;
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
	        ucentpr	*rp = static_cast<ucentpr *>(getprnam(name)) ;
	        if (rp) {
	            rs = prp->load(prbuf,prlen,rp) ;
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
/* end subroutine (ucgetpr::getpr_nam) */

int ucgetpr::getpr_num(ucentpr *prp,char *prbuf,int prlen) noex {
	int		rs = SR_INVALID ;
	if (num >= 0) {
	    errno = 0 ;
	    if constexpr (f_getprxxxr) {
	        cint	ec = getprnum_rp(prp,prbuf,prlen,num) ;
	        if (ec == 0) {
	            rs = prp->size() ;
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
	        ucentpr	*rp = static_cast<ucentpr *>(getprnum(num)) ;
	        if (rp) {
	            rs = prp->load(prbuf,prlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if-constexpr (selection) */
	    if constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (valid num) */
	return rs ;
}
/* end subroutine (ucgetpr::getpr_num) */


