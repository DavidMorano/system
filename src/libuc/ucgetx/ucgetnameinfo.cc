/* ucgetnameinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* manipulate host-address entry structures */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-02-03, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getnameinfo

	Description:
	This subroutine is a cleaned up version of |getnameinfo(3socket)|.

	Synopsis:
	int uc_getnameinfo(const SA *sap,int sal,char *hbuf,int hlen,
		char *sbuf,int slen,int fl) noex {

	Arguements:
	sap		SOCKADDR pointer
	sal		SOCDADDR length (address length)
	hbuf		host-name result buffer pointer
	hlen		host-name result buffer length (supplied)
	sbuf		serv-name result buffer pointer (can be NULL)
	slen		serv-name result buffer length (supplied)
	fl		optional request flags

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include        <errtimer.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SA		SOCKADDR


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int std_getnameinfo(con SA *,int,char *,int,char *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getnameinfo(const SA *sap,int sal,char *hbuf,int hlen,
		char *sbuf,int slen,int fl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (sap && hbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((sal > 0) && (hlen > 0) && (fl >= 0)) ylikely {
                errtimer	to_mfile	= utimeout[uto_mfile] ;
                errtimer        to_nfile	= utimeout[uto_nfile] ;
                errtimer        to_nomem	= utimeout[uto_nomem] ;
                errtimer        to_nospc	= utimeout[uto_nospc] ;
                errtimer        to_again	= utimeout[uto_again] ;
                errtimer        to_busy		= utimeout[uto_busy] ;
                reterr          r ;
		cauto		getinfo = std_getnameinfo ;
	        repeat {
		    if ((rs = getinfo(sap,sal,hbuf,hlen,sbuf,slen,fl)) < 0) {
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
	        if (rs >= 0) {
		    rl = lenstr(hbuf,hlen) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (uc_getnameinfo) */


/* local subroutines */

local int std_getnameinfo(con SA *sap,int sal,char *hbuf,int hlen,
		char *sbuf,int slen,int fl) noex {
	const socklen_t	hbl = socklen_t((hlen > 0) ? (hlen+1) : 0) ;
	const socklen_t	sbl = socklen_t((slen > 0) ? (slen+1) : 0) ;
	int		rs = SR_OK ;
	errno = 0 ;
	if (int rc = getnameinfo(sap,sal,hbuf,hbl,sbuf,sbl,fl) ; rc) {
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
                rs = SR_FAIL ;
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
                rs = (neg errno) ;
                break ;
            case EAI_BADHINTS:
                rs = SR_INVALID ;
                break ;
            case EAI_PROTOCOL:
                rs = SR_PROTONOSUPPORT ;
                break ;
            case EAI_OVERFLOW:
                rs = SR_OVERFLOW ;
                break ;
            default:
                rs = SR_NOANODE ;
                break ;
            } /* end switch */
	} /* end if (error) */
	return rs ;
} /* end subroutine (std_getnameinfo) */


