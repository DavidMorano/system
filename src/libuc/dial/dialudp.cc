/* dialudp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to dial out to a machine using UDP */
/* version %I% last-modified %G% */

#define	CF_PROTO	0		/* lookup protocol name */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialudp

	Description:
	This subroutine will dial out to an INET host using the UDP
	protocol and the optional port specified.

	Synopsis:
	int dialudp(cc *hostname,cc *portspec,int af,int to,int opts) noex

	Arguments:
	hostname	host to dial to
	portspec	port specification to use
	af		address family
	to		to ('>=0' mean use one, '-1' means do not)
	opts		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	Notes:
	If the given address family is unspecified, we cycle through:
		INET4
		INET6
	looking fo a connection.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<hostaddr.h>
#include	<getprotofamily.h>
#include	<getproto.h>
#include	<openaddrinfo.h>
#include	<iserror.h>
#include	<localmisc.h>

#include	"dialudp.h"


/* local defines */

#ifndef	ADDRINFO
#define	ADDRINFO	struct addrinfo
#endif

#define	PROTONAME	"udp"

#ifndef	CF_PROTO
#define	CF_PROTO	0		/* lookup protocol name */
#endif


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

static int	opendialudp(int,cchar *,cchar *,int) noex ;
static int	opendialudp_hint(ADDRINFO *,cchar *,cchar *,int) noex ;


/* local variables */

static constexpr int	pfs[] = {
	PF_INET4,
	PF_INET6,
	0
} ;

constexpr bool		f_proto = CF_PROTO ;


/* exported variables */


/* exported subroutines */

int dialudp(cchar *hostname,cchar *portspec,int af,int to,int) noex {
	int		rs = SR_FAULT ;
	if (hostname && portspec) {
	    rs = SR_INVALID ;
	    if (hostname[0] && portspec[0]) {
	        rs = opendialudp(af,hostname,portspec,to) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dialudp) */


/* local subroutines */

static int opendialudp(int af,cchar *hostname,cchar *portspec,int to) noex {
	ADDRINFO	hint{} ;
	int		rs = SR_OK ;
	if_constexpr (f_proto) {
	    cchar	*pn = PROTONAME ;
	    if ((rs = getproto_name(pn,-1)) >= 0) {
	        hint.ai_protocol = rs ;
	    }
	} else {
	    hint.ai_protocol = IPPROTO_UDP ;
	} /* end if_constexpr (f_proto) */
	if ((rs >= 0) && (af > 0)) {
	    if ((rs = getprotofamily(af)) >= 0) {
	        hint.ai_family = rs ; /* PF */
	    }
	}
/* do the spin */
	if (rs >= 0) {
	    if (hint.ai_family == PF_UNSPEC) {
	        rs = SR_NOTCONN ;
	        for (int i = 0 ; (pfs[i] > 0) && isFailConn(rs) ; i += 1) {
	            hint.ai_family = pfs[i] ;
	            rs = opendialudp_hint(&hint,hostname,portspec,to) ;
	            if (rs >= 0) break ;
	        } /* end for */
	    } else {
	        rs = opendialudp_hint(&hint,hostname,portspec,to) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (opendialudp) */

static int opendialudp_hint(ADDRINFO *hip,cchar *hs,cchar *ps,int to) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (hostaddr ha ; (rs = hostaddr_start(&ha,hs,ps,hip)) >= 0) {
	    hostaddr_cur	cur ;
	    if ((rs = hostaddr_curbegin(&ha,&cur)) >= 0) {
	        ADDRINFO	*aip ;
	        int		c = 0 ;
	        while ((rs1 = hostaddr_curenum(&ha,&cur,&aip)) >= 0) {
	            cint	aif = aip->ai_family ; /* PF */
	            bool	f = false ;
	            f = f || (hip->ai_family == 0) ;
	            f = f || (hip->ai_family == aif) ;
	            f = f || (aif == 0) ;
	            if (f) {
	                c += 1 ;
	                rs = openaddrinfo(aip,to) ;
	                fd = rs ;
	            } /* end if (protocol match) */
	            if (fd >= 0) break ;
	            if ((rs < 0) && (rs != SR_PFNOSUPPORT)) break ;
	        } /* end while */
	        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	        rs1 = hostaddr_curend(&ha,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	        if ((rs >= 0) && (c == 0)) rs = SR_HOSTUNREACH ;
	    } /* end if (cursor) */
	    rs1 = hostaddr_finish(&ha) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (hostaddr) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialudp_hint) */


