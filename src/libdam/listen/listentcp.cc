/* listentcp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to listen on a TCP port */
/* version %I% last-modified %G% */

#define	CF_PROTOLOOKUP	0		/* lookup protocol spec? */

/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	listentcp

	Descrption:
	This subroutine will bind to a port (any free one) and start
	to listen on that port for incoming connections.

	Synopsis:
	int listentcp(int af,cc *hostname,cc *portspec,int opts) noex

	Arguments:
	af		address family (numeric)
	hostname	hostname
	portspec	port (alpa or numeric)
	opts		opts

	Returns:
	>=0		listen FD
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getpf.h>		/* LIBUC */
#include	<getproto.h>		/* LIBUC */
#include	<hostaddr.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"listentcp.h"


/* local defines */

#ifndef	PROTONAME_TCP
#define	PROTONAME_TCP	"tcp"
#endif

#ifndef	ANYHOST
#define	ANYHOST		"anyhost"
#endif

#undef	PRNAME
#define	PRNAME		"LOCAL"

#ifndef	CF_PROTOLOOKUP
#define	CF_PROTOLOOKUP	0		/* lookup protocol spec? */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	listentcp_lookup(int,int,cchar *,cchar *,int) noex ;
local int	listentcp_try(ADDRINFO *,int) noex ;

local int	isFailListen(int) noex ;


/* local variables */

constexpr int	rsfails[] = {
	SR_ADDRINUSE,
	SR_ADDRNOTAVAIL,
	SR_AFNOSUPPORT,
	SR_ACCESS,
	0
} ; /* end array (rsfails) */

constexpr bool		f_protolookup = CF_PROTOLOOKUP ;


/* exported variables */


/* exported subroutines */

int listentcp(int af,cchar *hostname,cchar *portspec,int opts) noex {
	int		rs = SR_FAULT ;
	int		s = -1 ;
	if (portspec) ylikely {
	    rs = SR_INVALID ;
	    if (portspec[0] && (af >= 0)) ylikely {
	        int		proto = IPPROTO_TCP ;
	        cchar		*hp = hostname ;
                /* get the protocol number */
		if_constexpr (f_protolookup) {
	            rs = getproto_name(PROTONAME_TCP,-1) ;
	            proto = rs ;
		} /* end if_constexpr (f_protolookup) */
                /* host */
	        if (hp != nullptr) {
	            bool	f = false ;
	            f = f || (hp[0] == '\0') ;
	            f = f || (strcmp(hp,"*") == 0) ;
	            if (f) {
	                hp = ANYHOST ;
	            }
	        }
	        if (rs >= 0) ylikely {
	            if ((rs = getpf(af)) >= 0) {
	                cint	pf = rs ;
	                cchar	*ps = portspec ;
	                rs = listentcp_lookup(pf,proto,hp,ps,opts) ;
	                s = rs ;
	            } /* end if (getpf) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? s : rs ;
} /* end subroutine (listentcp) */


/* local subroutines */

local int listentcp_lookup(int pf,int proto,cc *hs,cc *ps,int opts) noex {
	ADDRINFO	hint{} ;
	HOSTADDR	ha ;
	HOSTADDR_CUR	hacur ;
	int		rs ;
	int		rs_last = 0 ;
	int		rs1 ;
	int		s = -1 ; /* return-value */
	{
	hint.ai_flags		= AI_PASSIVE ;
	hint.ai_family		= pf ;
	hint.ai_socktype	= SOCK_STREAM ;
	hint.ai_protocol	= proto ;
	}
	if ((rs = hostaddr_start(&ha,hs,ps,&hint)) >= 0) ylikely {
	    if ((rs = hostaddr_curbegin(&ha,&hacur)) >= 0) ylikely {
	        while ((rs >= 0) && (s < 0)) {
	            ADDRINFO	*aip ;
	            while ((rs = hostaddr_curenum(&ha,&hacur,&aip)) >= 0) {
	                if ((aip->ai_family == pf) || (pf == PF_UNSPEC)) break ;
	            } /* end while */
		    if (rs >= 0) {
	                if ((rs = listentcp_try(aip,opts)) >= 0) {
	                    s = rs ;
	                } else if (isFailListen(rs)) {
			    rs_last = rs ;
	                    rs = SR_OK ;
			}
	            } /* end if (ok) */
	        } /* end while */
	        rs1 = hostaddr_curend(&ha,&hacur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    rs1 = hostaddr_finish(&ha) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hostaddr) */
	if ((rs < 0) && (s >= 0)) {
	    u_close(s) ;
	}
	if ((rs == SR_NOENT) && (rs_last < 0)) {
	    rs = rs_last ;
	}
	return (rs >= 0) ? s : rs ;
} /* end subroutine (listentcp_lookup) */

local int listentcp_try(ADDRINFO *aip,int opts) noex {
	cint		proto = aip->ai_protocol ;
	cint		st = aip->ai_socktype ;
	cint		af = aip->ai_family ;
	int		rs ;
	int		s = -1 ;
	if ((rs = getpf(af)) >= 0) {
	    cint	spf = rs ;
	    if ((rs = u_socket(spf,st,proto)) >= 0) {
	        s = rs ;
	        if (opts != 0) {
	            cint	sz = szof(int) ;
	            cint	opt = SO_REUSEADDR ;
	            int		one = 1 ;
	            rs = u_setsockopt(s,SOL_SOCKET,opt,&one,sz) ;
	        }

#if	defined(IPPROTO_IPV6) && defined(IPV6_V6ONLY)
	        if ((rs >= 0) && (spf == PF_INET6)) {
	            cint	cmd = IPPROTO_IPV6 ;
	            cint	opt = IPV6_V6ONLY ;
	            cint	sz = szof(int) ;
	            int		one = 1 ;
	            rs = u_setsockopt(s,cmd,opt,&one,sz) ;
	        }
#endif /* defined(IPV6_V6ONLY) */

	        if (rs >= 0) {
	            SOCKADDR	*sap = aip->ai_addr ;
	            cint	sal = aip->ai_addrlen ;
	            if ((rs = u_bind(s,sap,sal)) >= 0) {
	                rs = u_listen(s,10) ;
	            } /* end if (bind) */
	        } /* end if (ok) */
	        if ((rs < 0) && (s >= 0)) u_close(s) ;
	    } /* end if (socket) */
	} /* end if (getpf) */
	return (rs >= 0) ? s : rs ;
} /* end subroutine (listentcp_try) */

local int isFailListen(int rs) noex {
	return isOneOf(rsfails,rs) ;
} /* end subroutine (isFailListen) */


