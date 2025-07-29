/* inetconv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert c-strings to binary INET addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-06, David A­D­ Morano
	This subroutine exists to make some sensible version out
	of the combination of |inet_addr(3n)| and |inet_pton(3socket)|.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inetpton

	Description:
	This subroutines converts a c-string (portable) representation
	of an INET address (either v4 or v6) into its binary form.

	Synopsis:
	int inetpton(void *addrbuf,int addrlen,int af,cchar *sp,int sl) noex

	Arguments:
	addrbuf		buffer to receive result
	addrlen		length of buffer to receive result
	af		address-family (AF) of source address
	sp		source address c-string pointer
	sl		source address c-string length

	Returns:
	>=0		the address family of the resulting address
	<0		error (system-return)


	Name:
	inetntop

	Description:
	This subroutines converts a network (binary) representation
	of an INET (either v4 or v6) address into its ASCII c-string
	(portable) form.

	Synopsis:
	int inetntop(char *rbuf,int rlen,int af,vcoid *binaddr) noex

	Arguments:
	rbuf		result string-buffer pointer
	rlen		result string-buffer length
	af		address-family (AF) of source address
	binaddr		byte-array representing the binary source address

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

	Notes:
	There is a mistake in the standard.
	The error-value SR_NOSPC can be returned when it should
	have been SR_OVERFLOW.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<strwcpy.h>
#include	<uinet.h>
#include	<inaddrbad.hh>
#include	<localmisc.h>

#include	"inetconv.h"


/* local defines */


/* imported namespaces */

using std::min ;
using std::max ;


/* local typedefs */


/* external subroutines */


/* forward references */

static inline int getaf(cchar *sp,int sl) noex {
	return (strnchr(sp,sl,':') != nullptr) ? AF_INET6 : AF_INET4 ;
}


/* local variables */

constexpr int		inet4addrlen = int(INET4ADDRLEN) ;
constexpr int		inet6addrlen = int(INET6ADDRLEN) ;
constexpr int		inetxaddrlen = max(inet4addrlen,inet6addrlen) ;

constexpr int		astrlen = int(INETX_ADDRSTRLEN) ;

constexpr in_addr_t	inaddrbad = mkinaddrbad() ;


/* exported variables */


/* exported subroutines */

int inetnetpton(void *dbuf,int dlen,int af,cchar *srcbuf,int srclen) noex {
	int		rs = SR_FAULT ;
	if (dbuf && srcbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (dlen > 0)) ylikely {
	        cchar	*sp{} ;
		rs = SR_DOM ;
	        if (int sl ; (sl = sfshrink(srcbuf,srclen,&sp)) > 0) {
	            char	astr[astrlen + 1] ;
	            strwcpy(astr,sp,min(sl,astrlen)) ;
	            if (af == AF_UNSPEC) {
	                af = getaf(sp,sl) ;
	            }
	            rs = uc_inetnetpton(dbuf,dlen,af,astr) ;
		} /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (inetnetpton) */

int inetpton(void *addrbuf,int addrlen,int af,cchar *srcbuf,int srclen) noex {
	int		rs = SR_FAULT ;
	if (addrbuf && srcbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (addrlen > 0)) ylikely {
	        cchar	*sp{} ;
		rs = SR_DOM ;
	        if (int sl ; (sl = sfshrink(srcbuf,srclen,&sp)) > 0) {
	            char	astr[astrlen + 1] ;
		    rs = SR_OVERFLOW ;
	            strwcpy(astr,sp,min(sl,astrlen)) ;
	            if (af == AF_UNSPEC) {
	                af = getaf(sp,sl) ;
	            }
		    switch (af) {
	            case AF_INET4:
	                if (addrlen >= inet4addrlen) {
	                    in_addr_t	a ;
	                    if ((a = inet_addr(astr)) != inaddrbad) {
	                        memcpy(addrbuf,&a,inet4addrlen) ;
				rs = inet4addrlen ;
			    } else {
				rs = SR_INVALID ;
		            }
		        }
			break ;
		    case AF_INET6:
	                if (addrlen >= inet6addrlen) {
	                    rs = uc_inetpton(addrbuf,af,astr) ;
		        }
			break ;
		    default:
			rs = SR_AFNOSUPPORT ;
			break ;
		    } /* end switch */
	        } /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? af : rs ;
}
/* end subroutine (inetpton) */

int inetntop(char *rbuf,int rlen,int af,cvoid *binaddr) noex {
	int		rs = SR_FAULT ;
	if (rbuf && binaddr) ylikely {
	    rs = SR_INVALID ;
	    if ((af >= 0) && (rlen > 0)) ylikely {
		rs = uc_inetntop(rbuf,rlen,af,binaddr) ;
		if (rs == SR_NOSPC) rs = SR_OVERFLOW ; 
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (inetntop) */


