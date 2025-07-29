/* isinetaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to determine if the name is an INET address a */
/* version %I% last-modified %G% */

#define	CF_INET6	1		/* test for both INET4 and INET6 */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isinetaddr

	Description:
	This subroutine checks if the given ASCII string represents
	an INET4 address or not.

	Synopsis:
	int isinetaddr(cchar *name) noex

	Arguments:
	name		address (character string) to test

	Returns:
	true		it is an INET address
	false		it is not

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<uinet.h>		/* |INETX_ADDRSTRLEN| */
#include	<inaddrbad.hh>
#include	<localmisc.h>

#include	"isinetaddr.h"


/* local defines */

#ifndef	CF_INET6
#define	CF_INET6	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	inetpton(char *,int,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr in_addr_t	inaddrbad = mkinaddrbad() ;

constexpr int		addrlen = INETX_ADDRSTRLEN ;
constexpr bool		f_inet6 = CF_INET6 ;


/* exported variables */


/* exported subroutines */

bool isinetaddr(cchar *name) noex {
	bool		f = false ;
	if_constexpr (f_inet6) {
	    cint	af = AF_UNSPEC ;
	    int		rs1 ;
	    char	addrbuf[addrlen + 1] ;
	    rs1 = inetpton(addrbuf,addrlen,af,name,-1) ;
	    f = (rs1 >= 0) ;
	} else {
	    f = (inet_addr(name) != inaddrbad) ;
	} /* end if_constexpr (f_inet6) */
	return f ;
}
/* end subroutine (isinetaddr) */


