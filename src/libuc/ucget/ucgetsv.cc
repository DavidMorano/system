/* ucgetsv */
/* lang=C++20 */

/* UNIX� C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This svogram was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getsvent
	uc_getsvnam
	uc_getsvnum

	Description:
	This subroutine is a platform independent subroutine to get an
	INET service (and retrieve it into the object |ucentsv|).

	Synopsis:
	int uc_getsvent(ucentsv *svp,char *svbuf,int svlen) noex
	int uc_getsvnam(ucentsv *svp,char *svbuf,int svlen,cc *n,cc *p) noex
	int uc_getsvnum(ucentsv *svp,char *svbuf,int svlen,int u,cc *p) noex

	Arguments:
	- svp		pointer to 'hostent' structure
	- svbuf		service-entry buffer pointer
	- svlen		service-entry buffer length
	- u		service-entry number
	- p		protocl c-string

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
#include	<errno.h>
#include	<cstdlib>
#include	<cstdint>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>
#include	<usysflag.h>

#include	"ucsyssv.h"
#include	"ucgetsv.h"


/* local defines */

#if defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0)
#define	F_GETSVXXXR	1
#else
#define	F_GETSVXXXR	0
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetsv ;
    typedef int (ucgetsv::*mem_f)(ucentsv *,char *,int) noex ;
    struct ucgetsv {
	mem_f		m ;
	cchar		*proto ;
	cchar		*name ;
	int		num ;
	ucgetsv(cchar *n,cchar *p = nullptr) noex : proto(p), name(n) { } ;
	ucgetsv(int n,cchar *p) noex : num(n), proto(p) { } ;
	int getsv_ent(ucentsv *,char *,int) noex ;
	int getsv_nam(ucentsv *,char *,int) noex ;
	int getsv_num(ucentsv *,char *,int) noex ;
	int operator () (ucentsv *,char *,int) noex ;
    } ; /* end struct (ucgetsv) */
}


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;

constexpr bool		f_getsvxxxr = F_GETSVXXXR ;


/* exported subroutines */

int uc_getsvbegin(int stayopen) noex {
	errno = 0 ;
	setservent(stayopen) ;
	return (- errno) ;
}
/* end subroutine (uc_getsvbegin) */

int uc_getsvend() noex {
	errno = 0 ;
	endservent() ;
	return (- errno) ;
}
/* end subroutine (uc_getsvend) */

int uc_getsvent(ucentsv *svp,char *svbuf,int svlen) noex {
	ucgetsv		svo(nullptr) ;
	svo.m = &ucgetsv::getsv_ent ;
	return svo(svp,svbuf,svlen) ;
}
/* end subroutine (uc_getsvent) */

int uc_getsvnam(ucentsv *svp,char *svbuf,int svlen,cchar *n,cchar *p) noex {
	ucgetsv		svo(n,p) ;
	svo.m = &ucgetsv::getsv_nam ;
	return svo(svp,svbuf,svlen) ;
}
/* end subroutine (uc_getsvnam) */

int uc_getsvnum(ucentsv *svp,char *svbuf,int svlen,int n,cchar *p) noex {
	ucgetsv		svo(n,p) ;
	svo.m = &ucgetsv::getsv_num ;
	return svo(svp,svbuf,svlen) ;
}
/* end subroutine (uc_getsvnum) */


/* local subroutines */

int ucgetsv::operator () (ucentsv *svp,char *svbuf,int svlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (svp && svbuf) {
	    rs = SR_OVERFLOW ;
	    if (svlen > 0) {
	        timecount	to_again(utimeout[uto_again]) ;
	        timecount	to_nomem(utimeout[uto_nomem]) ;
	        timecount	to_mfile(utimeout[uto_mfile]) ;
	        timecount	to_nfile(utimeout[uto_nfile]) ;
		bool		f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(svp,svbuf,svlen)) < 0) {
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
/* end subroutine (ucgetsv::operator) */

int ucgetsv::getsv_ent(ucentsv *svp,char *svbuf,int svlen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_getsvxxxr) {
	    cint	ec = getsvent_rp(svp,svbuf,svlen) ;
	    if (ec == 0) {
	        rs = svp->size() ;
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
	    ucentsv	*rp = static_cast<ucentsv *>(getsvent()) ;
	    if (rp) {
	        rs = svp->load(svbuf,svlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	if constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetsv::getsv_ent) */

int ucgetsv::getsv_nam(ucentsv *svp,char *svbuf,int svlen) noex {
	int		rs = SR_FAULT ;
	if (name && proto) {
	    errno = 0 ;
	    if constexpr (f_getsvxxxr) {
	        cint	ec = getsvnam_rp(svp,svbuf,svlen,name,proto) ;
	        if (ec == 0) {
	            rs = svp->size() ;
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
	        ucentsv	*rp = static_cast<ucentsv *>(getsvnam(name,proto)) ;
	        if (rp) {
	            rs = svp->load(svbuf,svlen,rp) ;
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
/* end subroutine (ucgetsv::getsv_nam) */

int ucgetsv::getsv_num(ucentsv *svp,char *svbuf,int svlen) noex {
	int		rs = SR_FAULT ;
	if (proto) {
	    rs = SR_INVALID ;
	    if ((num >= 0) && proto[0]) {
	        errno = 0 ;
	        if constexpr (f_getsvxxxr) {
	            cint	ec = getsvpor_rp(svp,svbuf,svlen,num,proto) ;
	            if (ec == 0) {
	                rs = svp->size() ;
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
	            ucentsv *rp = static_cast<ucentsv *>(getsvpor(num,proto)) ;
	            if (rp) {
	                rs = svp->load(svbuf,svlen,rp) ;
	            } else {
	                rs = (- errno) ;
	            }
	        } /* end if-constexpr (selection) */
	        if constexpr (f_sunos) {
		    if (rs == SR_BADF) rs = SR_NOENT ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetsv::getsv_num) */


