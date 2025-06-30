/* ucgetho SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_getho{x}

	Name:
	uc_gethobegin
	uc_gethoend
	uc_gethoent
	uc_gethonam
	uc_gethoadd

	Description:
	These subroutines are a platform independent subroutine to
	get an INET host (retrieved into an |ucentho| object).

	Synopsis:
	int uc_gethobegin(int stayopen) noex
	int uc_gethoend() noex
	int uc_gethoent(ucentho *pep,char *hobuf,int pelen) noex
	int uc_gethonam(ucentho *pep,char *hobuf,int pelen,cchar *name) noex
	int uc_gethoadd(ucentho *pep,char *hobuf,int pelen,
			int af,cv *ap,int al) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- hobuf		host-ent buffer pointer
	- pelen		host-ent buffer length
	- name		host-name to lookup
	- af		address family
	- ap		address pointer
	- al		address length

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
#include	<ucsysho.h>

#include	"ucgetho.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)
#define	F_GETHOXXXR	1
#else
#define	F_GETHOXXXR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* UCGET subgroup UC library type */


/* local typedefs */

typedef const void	cv ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetho ;
    typedef int (ucgetho::*mem_m)(ucentho *,char *,int) noex ;
    struct ucgetho {
	mem_m		m ;
	cchar		*name ;
	cvoid		*ap ;
	int		al ;
	int		af ;
	ucgetho(cchar *n,int f = 0,cvoid *a = 0,int l = 0) noex {
	    name = n ;
	    af = f ;
	    ap = a ;
	    al = l ;
	} ; /* end ctor */
	int getho_ent(ucentho *,char *,int) noex ;
	int getho_nam(ucentho *,char *,int) noex ;
	int getho_add(ucentho *,char *,int) noex ;
	int operator () (ucentho *,char *,int) noex ;
    } ; /* end struct (ucgetho) */
}


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;

constexpr bool		f_gethoxxxr = F_GETHOXXXR ;


/* exported variables */


/* exported subroutines */

int uc_gethobegin(int stayopen) noex {
	errno = 0 ;
	sethostent(stayopen) ;
	return (- errno) ;
}
/* end subroutine (uc_gethobegin) */

int uc_gethoend() noex {
	errno = 0 ;
	endhostent() ;
	return (- errno) ;
}
/* end subroutine (uc_gethoend) */

int uc_gethoent(ucentho *hop,char *hobuf,int holen) noex {
	ucgetho		hoo(nullptr) ;
	hoo.m = &ucgetho::getho_ent ;
	return hoo(hop,hobuf,holen) ;
}
/* end subroutine (uc_gethoent) */

int uc_gethonam(ucentho *hop,char *hobuf,int holen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        ucgetho		hoo(name) ;
	        hoo.m = &ucgetho::getho_nam ;
	        rs =hoo(hop,hobuf,holen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gethonam) */

int uc_gethoadd(ucentho *hop,char *hobuf,int holen,int af,cv *ap,int al) noex {
    	int		rs = SR_FAULT ;
	if (ap) {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (al > 0)) {
		ucgetho		hoo(nullptr,af,ap,al) ;
		hoo.m = &ucgetho::getho_add ;
	    	rs = hoo(hop,hobuf,holen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gethoadd) */


/* local subroutines */

int ucgetho::operator () (ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_FAULT ;
	if (hop && hobuf) {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; holen > 0) {
	        repeat {
	            if ((rs = (this->*m)(hop,hobuf,holen)) < 0) {
			rs = err(rs) ;
		    }
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetho::operator) */

int ucgetho::getho_ent(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
	errno = 0 ;
	if_constexpr (f_gethoxxxr) {
	    cerrno_t ec = gethoent_rp(hop,hobuf,holen) ;
	    if (ec == 0) {
	        rs = hop->size() ;
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
	    SYSDBHO	*ep = gethoent() ;
	    if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
	        rs = hop->load(hobuf,holen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetho::getho_ent) */

int ucgetho::getho_nam(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_gethoxxxr) {
            cerrno_t ec = gethonam_rp(hop,hobuf,holen,name) ;
            if (ec == 0) {
                rs = hop->size() ;
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
            SYSDBHO     *ep = gethonam(name) ;
            if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
                rs = hop->load(hobuf,holen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetho::getho_nam) */

int ucgetho::getho_add(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
        errno = 0 ;
        if_constexpr (f_gethoxxxr) {
            cerrno_t ec = gethoadd_rp(hop,hobuf,holen,af,ap,al) ;
            if (ec == 0) {
                rs = hop->size() ;
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
            SYSDBHO     *ep = gethoadd(af,ap,al) ;
            if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
                rs = hop->load(hobuf,holen,rp) ;
            } else {
                rs = (- errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
}
/* end subroutine (ucgetho::getho_add) */


