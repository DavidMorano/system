/* ucgetnw SUPPORT */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This nwogram was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getnwnam

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'nwotocol' (by name).

	Synopsis:
	int uc_getnwnam(ucentnw *pep,char *pebuf,int pelen,cchar *name) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- pebuf		nwoto-ent buffer pointer
	- pelen		nwoto-ent buffer length
	- name		nwotocol name to lookup

	Returns:
	0		entry was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_OVERFLOW	buffer overflow


	Name:
	uc_getnwnum

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'nwotocol' (by number).

	Synopsis:
	int uc_getnwnum(ucentnw *pep,char *pebuf,int pelen,
		int af,uint32_t a) noex

	Arguments:
	- pep		pointer to 'hostent' structure
	- rbuf		user supplied buffer to hold result
	- rlen		length of user supplied buffer
	- af		address familt (aka 'type')
	- a		address (aka 'number')

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
#include	<cstdint>
#include	<usystem.h>
#include	<localmisc.h>
#include	<usysflag.h>

#include	"ucsysnw.h"
#include	"ucgetnw.h"


/* local defines */

#if defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)
#define	F_GETNWXXXR	1
#else
#define	F_GETNWXXXR	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetnw ;
    typedef int (ucgetnw::*mem_f)(ucentnw *,char *,int) noex ;
    struct ucgetnw {
	mem_f		m ;
	cchar		*name ;
	uint32_t	num ;
	int		type ;
	ucgetnw(cchar *n,int t = 0,uint32_t u = 0) noex : name(n), type(t) { 
	    num = u ;
	} ;
	int getnw_ent(ucentnw *,char *,int) noex ;
	int getnw_nam(ucentnw *,char *,int) noex ;
	int getnw_num(ucentnw *,char *,int) noex ;
	int operator () (ucentnw *,char *,int) noex ;
    } ; /* end struct (ucgetnw) */
}


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;

constexpr bool		f_getnwxxxr = F_GETNWXXXR ;


/* exported variables */


/* exported subroutines */

int uc_getnwbegin(int stayopen) noex {
	errno = 0 ;
	setnetent(stayopen) ;
	return (- errno) ;
}
/* end subroutine (uc_getnwbegin) */

int uc_getnwend() noex {
	errno = 0 ;
	endnetent() ;
	return (- errno) ;
}
/* end subroutine (uc_getnwend) */

int uc_getnwent(ucentnw *nwp,char *nwbuf,int nwlen) noex {
	ucgetnw		nwo(nullptr) ;
	nwo.m = &ucgetnw::getnw_ent ;
	return nwo(nwp,nwbuf,nwlen) ;
}
/* end subroutine (uc_getnwent) */

int uc_getnwnam(ucentnw *nwp,char *nwbuf,int nwlen,cchar *name) noex {
	ucgetnw		nwo(name) ;
	nwo.m = &ucgetnw::getnw_nam ;
	return nwo(nwp,nwbuf,nwlen) ;
}
/* end subroutine (uc_getnwnam) */

int uc_getnwnum(ucentnw *nwp,char *nwbuf,int nwlen,int af,uint32_t num) noex {
	ucgetnw		nwo(nullptr,af,num) ;
	nwo.m = &ucgetnw::getnw_num ;
	return nwo(nwp,nwbuf,nwlen) ;
}
/* end subroutine (uc_getnwnum) */


/* local subroutines */

int ucgetnw::operator () (ucentnw *nwp,char *nwbuf,int nwlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (nwp && nwbuf) {
	    rs = SR_OVERFLOW ;
	    if (nwlen > 0) {
	        timecount	to_again(utimeout[uto_again]) ;
	        timecount	to_nomem(utimeout[uto_nomem]) ;
	        timecount	to_mfile(utimeout[uto_mfile]) ;
	        timecount	to_nfile(utimeout[uto_nfile]) ;
		bool		f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(nwp,nwbuf,nwlen)) < 0) {
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
/* end subroutine (ucgetnw::operator) */

int ucgetnw::getnw_ent(ucentnw *nwp,char *nwbuf,int nwlen) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if_constexpr (f_getnwxxxr) {
	    cint	ec = getnwent_rp(nwp,nwbuf,nwlen) ;
	    if (ec == 0) {
	        rs = nwp->size() ;
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
	    ucentnw	*rp = static_cast<ucentnw *>(getnwent()) ;
	    if (rp) {
	        rs = nwp->load(nwbuf,nwlen,rp) ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
}
/* end subroutine (ucgetnw::getnw_ent) */

int ucgetnw::getnw_nam(ucentnw *nwp,char *nwbuf,int nwlen) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    errno = 0 ;
	    if_constexpr (f_getnwxxxr) {
	        cint	ec = getnwnam_rp(nwp,nwbuf,nwlen,name) ;
	        if (ec == 0) {
	            rs = nwp->size() ;
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
	        ucentnw	*rp = static_cast<ucentnw *>(getnwnam(name)) ;
	        if (rp) {
	            rs = nwp->load(nwbuf,nwlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if_constexpr (selection) */
	    if_constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucgetnw::getnw_nam) */

int ucgetnw::getnw_num(ucentnw *nwp,char *nwbuf,int nwlen) noex {
	int		rs = SR_INVALID ;
	if (type >= 0) {
	    errno = 0 ;
	    if_constexpr (f_getnwxxxr) {
	        cint	ec = getnwnum_rp(nwp,nwbuf,nwlen,type,num) ;
	        if (ec == 0) {
	            rs = nwp->size() ;
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
	        ucentnw	*rp = static_cast<ucentnw *>(getnwnum(type,num)) ;
	        if (rp) {
	            rs = nwp->load(nwbuf,nwlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    } /* end if_constexpr (selection) */
	    if_constexpr (f_sunos) {
		if (rs == SR_BADF) rs = SR_NOENT ;
	    }
	} /* end if (valid num) */
	return rs ;
}
/* end subroutine (ucgetnw::getnw_num) */


