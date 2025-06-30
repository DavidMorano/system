/* ucgetsv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This svogram was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_getsv{x}

	Name:
	uc_getsvbegin
	uc_getsvent
	uc_getsvnam
	uc_getsvnum
	uc_getsvend

	Description:
	This subroutine is a platform independent subroutine to get an
	INET service (and retrieve it into the object |ucentsv|).

	Synopsis:
	int uc_getsvbegin(int so) noex
	int uc_getsvend() noex
	int uc_getsvent(ucentsv *svp,char *svbuf,int svlen) noex
	int uc_getsvnam(ucentsv *svp,char *svbuf,int svlen,cc *n,cc *p) noex
	int uc_getsvnum(ucentsv *svp,char *svbuf,int svlen,int u,cc *p) noex

	Arguments:
	- so		"stay-open" flag
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
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>
#include	<ucsyssv.h>

#include	"ucgetsv.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETSVXXXR) && (SYSHAS_GETSVXXXR > 0)
#define	F_GETSVXXXR	1
#else
#define	F_GETSVXXXR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetsv ;
    typedef int (ucgetsv::*mem_m)(ucentsv *,char *,int) noex ;
    struct ucgetsv {
	mem_m		m ;
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


/* exported variables */


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
    	int		rs = SR_FAULT ;
	if (n && p) {
	    if (n[0] && p[0]) {
		ucgetsv		svo(n,p) ;
		svo.m = &ucgetsv::getsv_nam ;
		rs = svo(svp,svbuf,svlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getsvnam) */

int uc_getsvnum(ucentsv *svp,char *svbuf,int svlen,int n,cchar *p) noex {
    	int		rs = SR_FAULT ;
	if (p) {
	    rs = SR_INVALID ;
	    if ((n >= 0) && p[0]) {
		ucgetsv		svo(n,p) ;
		svo.m = &ucgetsv::getsv_num ;
		rs = svo(svp,svbuf,svlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getsvnum) */


/* local subroutines */

int ucgetsv::operator () (ucentsv *svp,char *svbuf,int svlen) noex {
	int		rs = SR_FAULT ;
	if (svp && svbuf) {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; svlen > 0) {
	        repeat {
	            if ((rs = (this->*m)(svp,svbuf,svlen)) < 0) {
			rs = err(rs) ;
		    }
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetsv::operator) */

int ucgetsv::getsv_ent(ucentsv *svp,char *svbuf,int svlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	errno = 0 ;
	if_constexpr (f_getsvxxxr) {
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
	    SYSDBSV	*ep = getsvent() ;
	    if (ucentsv *rp = cast_static<ucentsv *>(ep) ; rp != np) {
	        rs = svp->load(svbuf,svlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetsv::getsv_ent) */

int ucgetsv::getsv_nam(ucentsv *svp,char *svbuf,int svlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_getsvxxxr) {
            cint    ec = getsvnam_rp(svp,svbuf,svlen,name,proto) ;
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
            SYSDBSV         *ep = getsvnam(name,proto) ;
            if (ucentsv *rp = cast_static<ucentsv *>(ep) ; rp != np) {
                rs = svp->load(svbuf,svlen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetsv::getsv_nam) */

int ucgetsv::getsv_num(ucentsv *svp,char *svbuf,int svlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_getsvxxxr) {
            cint        ec = getsvpor_rp(svp,svbuf,svlen,num,proto) ;
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
            SYSDBSV     *ep = getsvpor(num,proto) ;
            if (ucentsv *rp = cast_static<ucentsv *>(ep) ; rp != np) {
                rs = svp->load(svbuf,svlen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetsv::getsv_num) */


