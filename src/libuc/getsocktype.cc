/* getsocktype SUPPORT */
/* lang=C++20 */

/* get a socket type given a protocol number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsocktype

	Description:
	We retrieve a socket type (second argument to |socket(3xnet)|)
	given a protocol.

	Synopsis:
	int getsocktype(int proto) noex

	Arguments:
	proto		protocol number

	Returns:
	>=0		socket type number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */

struct socktype {
	int		proto ;
	int		type ;
} ;


/* forward references */


/* local variables */

static constexpr struct socktype	socktypes[] = {
	{ IPPROTO_TCP, SOCK_STREAM },
	{ IPPROTO_UDP, SOCK_DGRAM },
	{ IPPROTO_ICMP, SOCK_DGRAM },
	{ IPPROTO_EGP, SOCK_DGRAM },
	{ IPPROTO_GGP, SOCK_DGRAM },
	{ 0, 0 }
} ;


/* exported variables */


/* exported subroutines */

int getsocktype(int proto) noex {
	int		rs = SR_NOTFOUND ;
	int		i{} ;
	bool		f = false ;
	for (i = 0 ; socktypes[i].proto != 0 ; i += 1) {
	    f = (proto == socktypes[i].proto) ;
	    if (f) break ;
	} /* end for */
	if (f) {
	    rs = socktypes[i].type ;
	}
	return rs ;
}
/* end subroutine (getsocktype) */


