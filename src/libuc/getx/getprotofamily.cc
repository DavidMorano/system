/* getprotofamily SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a protocol family from an address family */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprotofamily

	Description:
	This subroutine will find a protocol family (if one exists)
	that corresponds with a specified address family.

	Synopsis:
	int getprotofamily(int af) noex

	Arguments:
	af		address family

	Returns:
	<0		no corresponding protocol family exists
	>=0		protocol family

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"getprotofamily.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */

struct typematch {
	cint		af, pf ;
} ; /* end struct (typematch) */


/* forward references */


/* local variables */

constexpr typematch	prototab[] = {
	{ PF_UNSPEC,	AF_UNSPEC },
	{ PF_INET,	AF_INET },
	{ PF_INET6,	AF_INET6 },
	{ PF_LOCAL,	AF_LOCAL },
	{ PF_UNIX,	AF_UNIX },
	{ PF_APPLETALK,	AF_APPLETALK },
	{ PF_IMPLINK,	AF_IMPLINK },
	{ PF_PUP,	AF_PUP },
	{ PF_CHAOS,	AF_CHAOS },
	{ PF_NS,	AF_NS },
	{ PF_NBS,	AF_NBS },
	{ PF_ECMA,	AF_ECMA },
	{ PF_DATAKIT,	AF_DATAKIT },
	{ PF_CCITT,	AF_CCITT },
	{ PF_SNA,	AF_SNA },
	{ PF_DECnet,	AF_DECnet },
	{ PF_DLI,	AF_DLI },
	{ PF_LAT,	AF_LAT },
	{ PF_HYLINK,	AF_HYLINK },
	{ PF_APPLETALK,	AF_APPLETALK },
	{ PF_NIT,	AF_NIT },
	{ PF_802,	AF_802 },
	{ PF_OSI,	AF_OSI },
	{ PF_X25,	AF_X25 },
	{ PF_OSINET,	AF_OSINET },
	{ PF_GOSIP,	AF_GOSIP },
	{ PF_IPX,	AF_IPX },
	{ PF_ROUTE,	AF_ROUTE },
	{ PF_LINK,	AF_LINK },
	{ PF_KEY,	AF_KEY },
	{ PF_NCA,	AF_NCA },
	{ PF_LOCAL,	AF_LOCAL },
	{ PF_ISDN,	AF_ISDN },
	{ PF_SYSTEM,	AF_SYSTEM },
	{ PF_NETBIOS,	AF_NETBIOS },
	{ PF_NDRV,	AF_NDRV },
	{ -1, -1 }
} ; /* end array */


/* exported variables */


/* exported subroutines */

int getprotofamily(int af) noex {
	int		i  = 0 ;
	bool		f = false ;
	while ((!f) && (prototab[i].af >= 0)) {
	    f = (prototab[i].af == af) ;
	    if (f) break ;
	    i += 1 ;
	} /* end while */
	return (f) ? prototab[i].pf : SR_AFNOSUPPORT ;
}
/* end subroutine (getprotofamily) */


