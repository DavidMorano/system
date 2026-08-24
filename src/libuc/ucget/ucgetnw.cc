/* ucgetnw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This nwogram was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_getnw{x}

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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<ucsysnw.h>		/* LIBUC */

#include	"ucgetnw.h"
#include	"ucgetxx.hh"


/* local defines */

#if defined(SYSHAS_GETNWXXXR) && (SYSHAS_GETNWXXXR > 0)
#define	F_GETNWXXXR	1
#else
#define	F_GETNWXXXR	0
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetnw ;
    typedef int (ucgetnw::*mem_m)(ucentnw *,char *,int) noex ;
    struct ucgetnw {
	mem_m		m ;
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
} /* end namespace */


/* forward references */


/* local variables */

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;

constexpr bool		f_getnwxxxr	= F_GETNWXXXR ;


/* exported variables */


/* exported subroutines */

int uc_getnwbegin(int stayopen) noex {
	errno = 0 ;
	setnetent(stayopen) ;
	return (neg errno) ;
} /* end subroutine (uc_getnwbegin) */

int uc_getnwend() noex {
	errno = 0 ;
	endnetent() ;
	return (neg errno) ;
} /* end subroutine (uc_getnwend) */

int uc_getnwent(ucentnw *nwp,char *nwbuf,int nwlen) noex {
	ucgetnw		nwo(nullptr) ;
	nwo.m = &ucgetnw::getnw_ent ;
	return nwo(nwp,nwbuf,nwlen) ;
} /* end subroutine (uc_getnwent) */

int uc_getnwnam(ucentnw *nwp,char *nwbuf,int nwlen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		ucgetnw		nwo(name) ;
		nwo.m = &ucgetnw::getnw_nam ;
		rs = nwo(nwp,nwbuf,nwlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getnwnam) */

int uc_getnwnum(ucentnw *nwp,char *nwbuf,int nwlen,int af,uint32_t num) noex {
    	int		rs = SR_INVALID ;
	if (af >= 0) ylikely {
	    ucgetnw	nwo(nullptr,af,num) ;
	    nwo.m = &ucgetnw::getnw_num ;
	    rs = nwo(nwp,nwbuf,nwlen) ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (uc_getnwnum) */


/* local subroutines */

int ucgetnw::operator () (ucentnw *nwp,char *nwbuf,int nwlen) noex {
	int		rs = SR_FAULT ;
	if (nwp && nwbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; nwlen > 0) ylikely {
	        repeat {
	            if ((rs = (this->*m)(nwp,nwbuf,nwlen)) < 0) {
			rs = err(rs) ;
	            } /* end if (error) */
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ucgetnw::operator) */

int ucgetnw::getnw_ent(ucentnw *nwp,char *nwbuf,int nwlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	if_constexpr (f_getnwxxxr) {
	    if ((rs = getnwent_rp(nwp,nwbuf,nwlen)) >= 0) {
	        rs = nwp->size() ;
	    } else {
		rs = (neg errno) ;
	    }
	} else {
	    SYSDBNW	*ep = getnwent() ;
	    if (ucentnw *rp = cast_static<ucentnw *>(ep) ; rp != np) {
	        rs = nwp->load(nwbuf,nwlen,rp) ;
	    } else {
	        rs = (neg errno) ;
	    }
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	}
	return rs ;
} /* end method (ucgetnw::getnw_ent) */

int ucgetnw::getnw_nam(ucentnw *nwp,char *nwbuf,int nwlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_getnwxxxr) {
            if ((rs = getnwnam_rp(nwp,nwbuf,nwlen,name)) >= 0) {
            } else {
                rs = (neg errno) ;
            }
        } else {
            SYSDBNW         *ep = getnwnam(name) ;
            if (ucentnw *rp = cast_static<ucentnw *>(ep) ; rp != np) {
                rs = nwp->load(nwbuf,nwlen,rp) ;
            } else {
                rs = (neg errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
} /* end method (ucgetnw::getnw_nam) */

int ucgetnw::getnw_num(ucentnw *nwp,char *nwbuf,int nwlen) noex {
    	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_getnwxxxr) {
            if ((rs = getnwnum_rp(nwp,nwbuf,nwlen,type,num)) >= 0) {
                rs = nwp->size() ;
            } else {
                rs = (neg errno) ;
            }
        } else {
            SYSDBNW         *ep = getnwnum(type,num) ;
            if (ucentnw *rp = cast_static<ucentnw *>(ep) ; rp != np) {
                rs = nwp->load(nwbuf,nwlen,rp) ;
            } else {
                rs = (neg errno) ;
            }
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        }
	return rs ;
} /* end method (ucgetnw::getnw_num) */


