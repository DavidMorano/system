/* ucgetho SUPPORT */
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
	uc_gethoent
	uc_gethonam
	uc_gethoadd

	Description:
	This subroutine is a platform independent subroutine to get an
	INET host (retrieved into an |ucentho| object).

	Synopsis:
	int uc_gethoent(ucentho *pep,char *hobuf,int pelen) noex
	int uc_gethonam(ucentho *pep,char *hobuf,int pelen,cchar *name) noex
	int uc_gethoadd(ucentho *pep,char *hobuf,int pelen,
			cv *ap,int al,int af) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- hobuf		host-ent buffer pointer
	- pelen		host-ent buffer length
	- name		host-name to lookup
	- ap		address pointer
	- al		address length
	- af		address family

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

#include	"ucsysho.h"
#include	"ucgetho.h"


/* local defines */

#if defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)
#define	F_GETHOXXXR	1
#else
#define	F_GETHOXXXR	0
#endif


/* local namespaces */


/* local typedefs */

typedef const void	cv ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetho ;
    typedef int (ucgetho::*mem_f)(ucentho *,char *,int) noex ;
    struct ucgetho {
	mem_f		m ;
	cchar		*name ;
	cvoid		*ap ;
	int		al ;
	int		af ;
	ucgetho(cchar *n,cvoid *a = 0,int l = 0,int f = 0) noex {
	    name = n ;
	    ap = a ;
	    al = l ;
	    af = f ;
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
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;

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
	ucgetho		hoo(name) ;
	hoo.m = &ucgetho::getho_nam ;
	return hoo(hop,hobuf,holen) ;
}
/* end subroutine (uc_gethonam) */

int uc_gethoadd(ucentho *hop,char *hobuf,int holen,cv *ap,int al,int af) noex {
	ucgetho		hoo(nullptr,ap,al,af) ;
	hoo.m = &ucgetho::getho_add ;
	return hoo(hop,hobuf,holen) ;
}
/* end subroutine (uc_gethoadd) */


/* local subroutines */

int ucgetho::operator () (ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (hop && hobuf) {
	    rs = SR_OVERFLOW ;
	    if (holen > 0) {
	        timecount	to_again(utimeout[uto_again]) ;
	        timecount	to_nomem(utimeout[uto_nomem]) ;
	        timecount	to_mfile(utimeout[uto_mfile]) ;
	        timecount	to_nfile(utimeout[uto_nfile]) ;
		bool		f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(hop,hobuf,holen)) < 0) {
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
/* end subroutine (ucgetho::operator) */

int ucgetho::getho_ent(ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_gethoxxxr) {
	    cint	ec = gethoent_rp(hop,hobuf,holen) ;
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
	    ucentho	*rp = static_cast<ucentho *>(gethoent()) ;
	    if (rp) {
	        rs = hop->load(hobuf,holen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if-constexpr (selection) */
	if constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetho::getho_ent) */

int ucgetho::getho_nam(ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if constexpr (f_gethoxxxr) {
	        cint	ec = gethonam_rp(hop,hobuf,holen,name) ;
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
	        ucentho	*rp = static_cast<ucentho *>(gethonam(name)) ;
	        if (rp) {
	            rs = hop->load(hobuf,holen,rp) ;
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
/* end subroutine (ucgetho::getho_nam) */

int ucgetho::getho_add(ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_FAULT ;
	if (ap) {
	    rs = SR_INVALID ;
	    if ((al >= 0) && (af >= 0)) {
	        errno = 0 ;
	        if constexpr (f_gethoxxxr) {
	            cint	ec = gethoadd_rp(hop,hobuf,holen,ap,al,af) ;
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
	            ucentho *rp = static_cast<ucentho *>(gethoadd(ap,al,af)) ;
	            if (rp) {
	                rs = hop->load(hobuf,holen,rp) ;
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
/* end subroutine (ucgetho::getho_add) */


