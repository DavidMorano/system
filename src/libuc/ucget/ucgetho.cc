/* ucgetho SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

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
	uc_gethobeg
	uc_gethoend
	uc_gethoent
	uc_gethonam
	uc_gethoadd

	Description:
	These subroutines are a platform independent subroutine to
	get an INET host (retrieved into an |ucentho| object).

	Synopsis:
	int uc_gethobeg(int stayopen) noex
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
#include	<deb.hh>		/* LIBU */
#include	<dprint.hh>		/* LIBU */
#include	<ucsysho.h>		/* LIBUC */

#include	"ucgetho.h"
#include	"ucgetxx.hh"

#pragma		GCC dependency		"mod/deb.ccm"

import deb ;				/* debugging */

/* local defines */

#if defined(SYSHAS_GETHOXXXR) && (SYSHAS_GETHOXXXR > 0)
#define	F_GETHOXXXR	1
#else
#define	F_GETHOXXXR	0
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using ucget::ucgeter ;			/* UCGET subgroup UC library type */


/* local typedefs */

typedef cvoid	cv ;


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
	int getho_ent	(ucentho *,char *,int) noex ;
	int getho_nam	(ucentho *,char *,int) noex ;
	int getho_add	(ucentho *,char *,int) noex ;
	int operator ()	(ucentho *,char *,int) noex ;
    } ; /* end struct (ucgetho) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_gethoxxxr	= F_GETHOXXXR ;


/* exported variables */


/* exported subroutines */

int uc_gethobeg(int stayopen) noex {
	errno = 0 ;
	sethostent(stayopen) ;
	return (neg errno) ;
} /* end subroutine (uc_gethobeg) */

int uc_gethoend() noex {
	errno = 0 ;
	endhostent() ;
	return (neg errno) ;
} /* end subroutine (uc_gethoend) */

int uc_gethoent(ucentho *hop,char *hobuf,int holen) noex {
	ucgetho		hoo(nullptr) ;
	hoo.m = &ucgetho::getho_ent ;
	return hoo(hop,hobuf,holen) ;
} /* end subroutine (uc_gethoent) */

int uc_gethonam(ucentho *hop,char *hobuf,int holen,cchar *name) noex {
    	int		rs = SR_FAULT ;
	DEBPRINTF("ent n=%s\n",name) ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
	        ucgetho		hoo(name) ;
	        hoo.m = &ucgetho::getho_nam ;
	DEBPRINTF("go\n") ;
	        rs = hoo(hop,hobuf,holen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (uc_gethonam) */

int uc_gethoadd(ucentho *hop,char *hobuf,int holen,int af,cv *ap,int al) noex {
    	int		rs = SR_FAULT ;
	if (ap) ylikely {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (al > 0)) ylikely {
		ucgetho		hoo(nullptr,af,ap,al) ;
		hoo.m = &ucgetho::getho_add ;
	    	rs = hoo(hop,hobuf,holen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_gethoadd) */


/* local subroutines */

int ucgetho::operator () (ucentho *hop,char *hobuf,int holen) noex {
	int		rs = SR_FAULT ;
	if (hop && hobuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (ucgeter err ; holen > 0) ylikely {
	        repeat {
	            if ((rs = (this->*m)(hop,hobuf,holen)) < 0) {
			rs = err(rs) ;
		    } /* end */
	        } until ((rs >= 0) || err.fexit) ;
	    } /* end if (buffer length non-negative) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ucgetho::operator) */

int ucgetho::getho_ent(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
	if_constexpr (f_gethoxxxr) {
	    if ((rs = gethoent_rp(hop,hobuf,holen)) >= 0) {
	        rs = hop->size() ;
	    } else {
		rs = (neg errno) ;
	    } /* end */
	} else {
	    SYSDBHO	*ep = gethoent() ;
	    if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
	        rs = hop->load(hobuf,holen,rp) ;
	    } else {
	        rs = (neg errno) ;
	    } /* end */
	} /* end if_constexpr (selection) */
	if_constexpr (f_sunos) {
	    if (rs == SR_BADF) rs = SR_NOENT ;
	} /* end */
	return rs ;
} /* end method (ucgetho::getho_ent) */

int ucgetho::getho_nam(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
	DEBPRINTF("ent n=%s\n",name) ;
        if_constexpr (f_gethoxxxr) {
	DEBPRINTF("mid1 \n") ;
            if ((rs = gethonam_rp(hop,hobuf,holen,name)) >= 0) {
	DEBPRINTF("mid1 sz-bef rs=%d\n",rs) ;
                rs = hop->size() ;
	DEBPRINTF("mid1 sz-aft rs=%d\n",rs) ;
            } else {
                rs = (neg errno) ;
            } /* end */
        } else {
            SYSDBHO     *ep = gethonam(name) ;
	DEBPRINTF("mid2\n") ;
            if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
	DEBPRINTF("mid2 sz-bef \n") ;
                rs = hop->load(hobuf,holen,rp) ;
	DEBPRINTF("mid2 sz-aft rs=%d\n",rs) ;
            } else {
                rs = (neg errno) ;
            } /* end */
        } /* end if_constexpr (selection) */
	DEBPRINTF("mid3 rs=%d\n",rs) ;
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        } /* end */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (ucgetho::getho_nam) */

int ucgetho::getho_add(ucentho *hop,char *hobuf,int holen) noex {
	cnullptr	np{} ;
	int		rs ;
        if_constexpr (f_gethoxxxr) {
            if ((rs = gethoadd_rp(hop,hobuf,holen,af,ap,al)) >= 0) {
                rs = hop->size() ;
            } else {
                rs = (neg errno) ;
            } /* end */
        } else {
            SYSDBHO     *ep = gethoadd(af,ap,al) ;
            if (ucentho *rp = cast_static<ucentho *>(ep) ; rp != np) {
                rs = hop->load(hobuf,holen,rp) ;
            } else {
                rs = (neg errno) ;
            } /* end */
        } /* end if_constexpr (selection) */
        if_constexpr (f_sunos) {
            if (rs == SR_BADF) rs = SR_NOENT ;
        } /* end */
	return rs ;
} /* end method (ucgetho::getho_add) */


