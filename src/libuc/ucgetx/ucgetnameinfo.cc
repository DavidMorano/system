/* ucgetnameinfo SUPPORT */
/* external subroutines */


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
	sbuf		serv-name result buffer pointer
	slen		serv-name result buffer length (supplied)
	fl		optional request flags

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strnlen(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	SA		SOCKADDR


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getnameinfo(const SA *sap,int sal,char *hbuf,int hlen,
		char *sbuf,int slen,int fl) noex {
	const socklen_t	hbl = socklen_t((hlen > 0) ? (hlen+1) : 0) ;
	const socklen_t	sbl = socklen_t((slen > 0) ? (slen+1) : 0) ;
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (sap && hbuf && sbuf) {
	    rs = SR_INVALID ;
	    if (fl >= 0) {
	        int		to_again = utimeout[uto_again] ;
	        int		rc ;
	        bool	f_exit = false ;
	        repeat {
	            rs = SR_OK ;
	            errno = 0 ;
	            rc = getnameinfo(sap,sal,hbuf,hbl,sbuf,sbl,fl) ;
	            if (rc != 0) {
	                switch (rc) {
	                case EAI_ADDRFAMILY:
	                    rs = SR_AFNOSUPPORT ;
		            f_exit = true ;
	                    break ;
	                case EAI_AGAIN:
	                    if (to_again-- > 0) {
			        msleep(1000) ;
		            } else {
	                        rs = SR_AGAIN ;
			        f_exit = true ;
		            }
	                    break ;
	                case EAI_BADFLAGS:
	                    rs = SR_INVALID ;
		            f_exit = true ;
	                    break ;
	                case EAI_FAIL:
	                    rs = SR_FAIL ;
		            f_exit = true ;
	                    break ;
	                case EAI_FAMILY:
	                    rs = SR_PFNOSUPPORT ;
		            f_exit = true ;
	                    break ;
	                case EAI_MEMORY:
	                    rs = SR_NOMEM ;
		            f_exit = true ;
	                    break ;
	                case EAI_NODATA:
	                    rs = SR_NODATA ;
		            f_exit = true ;
	                    break ;
	                case EAI_NONAME:
	                    rs = SR_NOTFOUND ;
		            f_exit = true ;
	                    break ;
	                case EAI_SERVICE:
	                    rs = SR_ADDRNOTAVAIL ;
		            f_exit = true ;
	                    break ;
	                case EAI_SOCKTYPE:
	                    rs = SR_SOCKTNOSUPPORT ;
		            f_exit = true ;
	                    break ;
	                case EAI_SYSTEM:
	                    rs = (- errno) ;
	                    switch (rs) {
	                    case SR_AGAIN:
	                        if (to_again-- > 0) {
			            msleep(1000) ;
			        } else {
			            f_exit = true ;
			        }
	                        break ;
		            default:
			        f_exit = true ;
			        break ;
	                    } /* end switch */
	                    break ;
	                case EAI_BADHINTS:
	                    rs = SR_INVALID ;
		            f_exit = true ;
	                    break ;
	                case EAI_PROTOCOL:
	                    rs = SR_PROTONOSUPPORT ;
		            f_exit = true ;
	                    break ;
	                case EAI_OVERFLOW:
	                    rs = SR_OVERFLOW ;
		            f_exit = true ;
	                    break ;
	                default:
	                    rs = SR_NOANODE ;
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (some sort of error condition) */
	        } until ((rs >= 0) || f_exit) ;
	        rs = strnlen(hbuf,hlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_getnameinfo) */


