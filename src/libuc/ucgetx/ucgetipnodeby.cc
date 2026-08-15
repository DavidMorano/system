/* ucgetipnodeby SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* subroutine to get a single host entry by name (raw) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getipnodebyname
	uc_getipnodebyaddr
	uc_hostentfree

	Description:
	These subroutines provide a platform independent way to
	retreive host (mode) information given either: a hostname
	and address-family, or an IP address and address-family.

	Synopsis:
	int uc_getipnodebyname(HE **rpp,cc *hostname,int af,int fl) noex
	int uc_getipnodebyaddr(HE **rpp,cvoid *ap,int al,int af) noex
	int uc_hostentfree(HE *hep) noex

	Arguments:
	- hepp		pointer to get a HOSTENT pointer
	- hostname	hostname to lookup
	- ap		address pointer
	- al		address length
	- af		desired address family
	- fl		optional flags

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_PROTO	this really means some unknown error happened 
	SR_AFNOSUPPORT	address-family not-found
	SR_DOWN		some sort of fatæl "unrecoverable" error
	-		other error (system-return)

	Notes:
	Some special errors returned by the underlying library calls are:
		NETDB_INTERNAL		see |errno|
		NETDB_SUCCESS		success
		NETDB_TRYAGAIN		try the operation again
		NETDB_NOTFOUND		host (entry) was not found
		NETDB_NOADDRESS		address-family not found
		NETDB_NORECOVERY	mean total fatal error (of some kind)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |msleep(3u)| */
#include        <errtimer.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"
#include	"ucgetipnodeby.h"


/* local defines */

#define	HE		hostent

#ifndef	NETDB_INTERNAL			/* indicates to see |errno| */
#define	NETDB_INTERNAL	-1
#endif
#ifndef	NETDB_SUCCESS			/* indicates success */
#define	NETDB_SUCCESS	0
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct getter ;
    typedef int (getter::*getter_m)(HE **,int) noex ;
    struct getter {
	getter_m	m ;
	cchar		*hn ;
	cvoid		*ap ;
	int		al ;
	int		fl ;
	int callname(HE **,int) noex ;
	int calladdr(HE **,int) noex ;
	getter(cchar *h,int f) noex : hn(h), fl(f) {
	    m = &getter::callname ;
	} ;
	getter(cvoid *p,int l) noex : ap(p), al(l) {
	    m = &getter::calladdr ;
	} ;
	int operator () (HE **,int) noex ;
    } ; /* end struct (getter) */
} /* end namespace */


/* forward references */

local sysret_t	std_getipnodebyname(HE **,cc *,int,int) noex ;
local sysret_t	std_getipnodebyaddr(HE **,cvoid *,int,int) noex ;
local int	errcvt(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getipnodebyname(HOSTENT **hepp,cchar *hn,int af,int fl) noex {
	int		rs = SR_FAULT ;
	if (hepp && hn) {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (fl >= 0)) {
		getter go(hn,fl) ;
		rs = go(hepp,af) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getipnodebyname) */

int uc_getipnodebyaddr(HOSTENT **hepp,cvoid *ap,int al,int af) noex {
	int		rs = SR_FAULT ;
	if (hepp && ap) {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (al >= 0)) {
		getter go(ap,al) ;
		rs = go(hepp,af) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getipnodebyaddr) */

int uc_hostentfree(HOSTENT *hep) noex {
	int		rs = SR_FAULT ;
	if (hep) {
	    rs = SR_OK ;
	    freehostent(hep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_hostentfree) */


/* local subroutines */

int getter::operator () (HE **hepp,int af) noex {
    	int		rs = SR_OK ;
        errtimer        to_mfile        = utimeout[uto_mfile] ;
        errtimer        to_nfile        = utimeout[uto_nfile] ;
        errtimer        to_nomem        = utimeout[uto_nomem] ;
        errtimer        to_nospc        = utimeout[uto_nospc] ;
        errtimer        to_again        = utimeout[uto_again] ;
        errtimer        to_busy         = utimeout[uto_busy] ;
        reterr          r ;
        repeat {
            if ((rs = (this->*m)(hepp,af)) < 0) {
                r(rs) ;                 /* <- default causes exit */
                switch (rs) {
                case SR_MFILE:
                    r = to_mfile(rs) ;
                    break ;
                case SR_NFILE:
                    r = to_nfile(rs) ;
                    break ;
                case SR_NOMEM:
                    r = to_nomem(rs) ;
                    break ;
                case SR_NOSPC:
                    r = to_nospc(rs) ;
                    break ;
                case SR_AGAIN:
                    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
                    r = to_busy(rs) ;
                    break ;
                case SR_INTR:
                    r(false) ;
                    break ;
                } /* end switch */
                rs = r ;
            } /* end if (error) */
        } until ((rs >= 0) || r.fexit) ;
	return rs ;
} /* end method (getter::operator) */

int getter::callname(HE **hepp,int af) noex {
	return std_getipnodebyname(hepp,hn,af,fl) ;
} /* end method (getter::callname) */

int getter::calladdr(HE **hepp,int af) noex {
	return std_getipnodebyaddr(hepp,ap,al,af) ;
} /* end method (getter::calladdr) */

local sysret_t std_getipnodebyname(HE **hepp,cc *hn,int af,int fl) noex {
	cnullptr	np{} ;
    	int		rs = SR_OK ;
	int		herr = 0 ;
	errno = 0 ;
	if (HE *lp ; (lp = getipnodebyname(hn,af,fl,&herr)) == np) {
	    rs = errcvt(herr) ;
	} else {
	    *hepp = lp ;
	} /* end error */
	return rs ;
} /* end subroutine (std_getipnodebyname) */

local sysret_t std_getipnodebyaddr(HE **hepp,cvoid *ap,int al,int af) noex {
    	cnullptr	np{} ;
	csize		asize = size_t(al) ;
	int		rs = SR_OK ;
	int		herr = 0 ;
	errno = 0 ;
	if (HE *lp ; (lp = getipnodebyaddr(ap,asize,af,&herr)) == np) {
	    rs = errcvt(herr) ;
	} else {
	    *hepp = lp ;
	} /* end error */
	return rs ;
} /* end subroutine (std_getipnodebyaddr) */

local int errcvt(int herr) noex {
    	int		rs = SR_OK ;
	switch (herr) {
	case NETDB_SUCCESS:
	    break ;
	case NETDB_TRYAGAIN:
	    rs = SR_AGAIN ;
	    break ;
	case NETDB_NOTFOUND:
	    rs = SR_NOTFOUND ;
	    break ;
	case NETDB_NOADDRESS:
	    rs = SR_AFNOSUPPORT ;
	    break ;
	case NETDB_NORECOVERY:
	    rs = SR_DOWN ;
	    break ;
	case NETDB_INTERNAL:
	    rs = (- errno) ;
	    break ;
	default:
	    rs = SR_PROTO ;
	    break ;
	} /* end switch */
	return rs ;
} /* end subroutine (errcvt) */


