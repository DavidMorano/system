/* ucaddrinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* manipulate host-address entry structures */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_addrinfoget
	uc_addrinfofree

	Description:
	This is a nicer version of the standard |getaddrinfo(3socket)|
	subroutine.

	Synopsis:
	int uc_addrinfoget(cc *hn,cc *svc,ADDRINFO *hintp,ADDRINFO **rpp) noex
	int uc_addrinfofree(ADDRINFO *aip) noex

	Arguments:
	hn		host-name
	svc		service-name
	hintp		pointer to a 'addrinfo' structure (as a hint)
	rpp		pointer to pointer to 'addrinfo' structure

	Returns:
	>=0		OK
	<0		error (system-return)

	Name:
	uc_addrinfofree

	Description:
	These subroutine is a cleaned up version of
	|getaddrinfo(3socket)|.

	Synopsis:
	int uc_eaddrinfofree(ADDRINFO *aip) noex

	Arguments:
	aip		pointer to 'addrinfo' structure

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include        <errtimer.hh>
#include	<localmisc.h>

#include	"ucaddrinfo.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

using ai = ADDRINFO ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int std_getaddinfo(cc *,cc *,con ai *,ai **) noex ;


/* local variables */


/* exported subroutines */

int uc_addrinfoget(cc *hn,cc *svc,con ai *hintp,ai **rpp) noex {
    	int		rs = SR_FAULT ;
	if (hn || svc) { /* <- either must be non-NULL */
	    rs = SR_INVALID ;
	    if (hn[0] && svc[0]) {
                errtimer    to_again        = utimeout[uto_again] ;
                errtimer    to_busy         = utimeout[uto_busy] ;
                errtimer    to_nomem        = utimeout[uto_nomem] ;
                errtimer    to_nosr         = utimeout[uto_nosr] ;
                errtimer    to_nobufs       = utimeout[uto_nobufs] ;
                errtimer    to_mfile        = utimeout[uto_mfile] ;
                errtimer    to_nfile        = utimeout[uto_nfile] ;
                errtimer    to_nolck        = utimeout[uto_nolck] ;
                errtimer    to_nospc        = utimeout[uto_nospc] ;
                errtimer    to_dquot        = utimeout[uto_dquot] ;
                errtimer    to_io           = utimeout[uto_io] ;
		errtimer    to_inprogress   = utimeout[uto_inprogress] ;
                reterr      r ;
	        repeat {
	            if ((rs = std_getaddinfo(hn,svc,hintp,rpp)) < 0) {
                        r(rs) ;		/* <- default causes exit */
                        switch (rs) {
                        case SR_AGAIN:
                            r = to_again(rs) ;
                            break ;
                        case SR_BUSY:
                            r = to_busy(rs) ;
                            break ;
                        case SR_NOMEM:
                            r = to_nomem(rs) ;
                            break ;
                        case SR_NOSR:
                            r = to_nosr(rs) ;
                            break ;
                        case SR_NOBUFS:
                            r = to_nobufs(rs) ;
                            break ;
                        case SR_MFILE:
                            r = to_mfile(rs) ;
                            break ;
                        case SR_NFILE:
                            r = to_nfile(rs) ;
                            break ;
                        case SR_NOLCK:
                            r = to_nolck(rs) ;
                            break ;
                        case SR_NOSPC:
                            r = to_nospc(rs) ;
                            break ;
                        case SR_DQUOT:
                            r = to_dquot(rs) ;
                            break ;
                        case SR_IO:
                            r = to_io(rs) ;
                            break ;
                        case SR_INPROGRESS: /* who thought up this? */
                            r = to_inprogress(rs) ;
                            break ;
                        case SR_INTR:
                            r(false) ;
                            break ;
                        } /* end switch */
                        rs = r ;
                    } /* end if (callstd) */
                } until ((rs >= 0) || r.fexit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_addrinfoget) */

int uc_addrinfofree(ADDRINFO *aip) noex {
	int		rs = SR_FAULT ;
	if (aip) {
	    freeaddrinfo(aip) ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (uc_addrinfofree) */


/* local subroutines */

local int std_getaddinfo(cc *hn,cc *svc,con ai *hintp,ai **rpp) noex {
    	int		rs = SR_OK ;
	errno = 0 ;
	if (int rc ; (rc = getaddrinfo(hn,svc,hintp,rpp)) != 0) {
            switch (rc) {
            case EAI_ADDRFAMILY:
                rs = SR_AFNOSUPPORT ;
                break ;
            case EAI_AGAIN:
                rs = SR_AGAIN ;
                break ;
            case EAI_BADFLAGS:
                rs = SR_INVALID ;
                break ;
            case EAI_FAIL:
                rs = SR_FAIL;
                break ;
            case EAI_FAMILY:
                rs = SR_PFNOSUPPORT ;
                break ;
            case EAI_MEMORY:
                rs = SR_NOMEM ;
                break ;
            case EAI_NODATA:
                rs = SR_NODATA ;
                break ;
            case EAI_NONAME:
                rs = SR_NOTFOUND ;
                break ;
            case EAI_SERVICE:
                rs = SR_ADDRNOTAVAIL ;
                break ;
            case EAI_SOCKTYPE:
                rs = SR_SOCKTNOSUPPORT ;
                break ;
            case EAI_SYSTEM:
                rs = (- errno) ;
                break ;
            default:
                rs = SR_NOANODE ;
                break ;
            } /* end switch */
        } /* end if (some sort of error condition) */
	return rs ;
} /* end subroutine (std_getaddrinfo) */


