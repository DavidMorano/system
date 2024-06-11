/* isinetaddr SUPPORT */
/* lang=C++20 */

/* subroutine to determine if the name is an INET address a */
/* version %I% last-modified %G% */

#define	CF_INET6	1		/* test for both INET4 and INET6 */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<inaddrbad.hh>
#include	<localmisc.h>

#include	"isinetaddr.h"


/* local defines */

#ifndef	ADDRBUFLEN
#define	ADDRBUFLEN	64		/* size (in bytes) of c-str to hold */
#endif

#ifndef	CF_INET6
#define	CF_INET6	0
#endif


/* external subroutines */

extern int	inetpton(char *,int,int,cchar *,int) noex ;


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */

constexpr bool			f_inet6 = CF_INET6 ;

static constexpr in_addr_t	inaddrbad = mkinaddrbad() ;


/* exported variables */


/* exported subroutines */

bool isinetaddr(cchar *name) noex {
	bool		f = false ;
	if constexpr (f_inet6) {
	    cint	af = AF_UNSPEC ;
	    int		rs1 ;
	    char	addrbuf[ADDRBUFLEN + 1] ;
	    rs1 = inetpton(addrbuf,ADDRBUFLEN,af,name,-1) ;
	    f = (rs1 >= 0) ;
	} else {
	    f = (inet_addr(name) != inaddrbad) ;
	} /* end if_constexpr (f_inet6) */
	return f ;
}
/* end subroutine (isinetaddr) */


