/* inetping SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PING an INET machine */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time */
#define	CF_DEBUGPING	0		/* debug PING */
#define	CF_DEBUGINT	0		/* compile |makeint()| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inetping

	Description:
	This subroutine performs an INET ICMP 'ping' of the specified
	host. An optional timeout can be specified as well.

	Synopsis:
	int inetping(cchar *host,int to) noex

	Arguments:
	host		character string of host name to 'ping'
	to		number of secnods to wait for a 'ping' response

	Returns:
	>=0		success
	<0		failure

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<csignal>		/* CSTD |SIG{x}| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uinet.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<hostent.h>		/* LIBUC */
#include	<inetaddr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC |spawnproc_con(3uc)| */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<sfx.h>			/* LIBUC |sfnext(3uc)| */
#include	<ctdec.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"inetping.h"

#pragma		GCC dependency		"mod/deb.ccm"

import deb ;

/* local defines */

#ifndef	ADDR_NOT
#define	ADDR_NOT	((uint) (~ 0))
#endif

#ifndef	DOTBUFLEN
#define	DOTBUFLEN	INETX_ADDRSTRLEN /* INET dotted-decimal length */
#endif

#define	TOBUFLEN	DIGBUFLEN

#undef	PROGNAME_PING
#define	PROGNAME_PING	"INETPING"

#if	CF_DEBUGPING
#define	TO_PING		10
#define	TO_MORETIME	2
#define	TO_READ		2
#else
#define	TO_PING		(5*60)
#define	TO_MORETIME	10
#define	TO_READ		30
#endif

#define	TO_CHILDEXIT	5		/* time to wait for child exit */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0
#endif

#define CDEBPR(FMT, ...) \
	({								\
	    int rc = 0 ;						\
	    if_constexpr (f_debug) {					\
    	        rc = debprintf(__func__, FMT __VA_OPT__(,) __VA_ARGS__) ; \
	    }								\
	    rc ;							\
	})


/* imported namespaces */


/* local typedefs */

typedef in4_addr_t	*addrp ;


/* external subroutines */

extern "C" {
    extern int uc_readlnto(int,void *,int,int) noex ;
} /* end extern (C) */

extern "C" {
    extern int	getheour(cchar *,char *,HOSTENT *,char *,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

namespace {
    struct suber {
	HOSTENT		he{} ;
	cchar		*rhost ;	/* supplied argument */
	cchar		*prog{} ;
	char		*hebuf{} ;
	in4_addr_t	a{} ;
	int		to ;		/* supplied argument */
	int		helen ;
	bool		fnumeric{} ;
	suber(cchar *r,int t) noex : rhost(r), to(t) { } ;
	operator int () noex ;
	int getaddr() noex ;
	int getprog() noex ;
	int getping() noex ;
    } ; /* end sturct (suber) */
} /* end namespace */


/* forward reference */

local int	pingone		(cchar *,const in4_addr_t *,int) noex ;
local int	pingoneresp	(int,int) noex ;
local int	pingoneparse	(cchar *,int) noex ;

#if	CF_DEBUG & CF_DEBUGINT
local int	makeint(void *) noex ;
#endif


/* local variables */

constexpr cpcchar	pings[] = {
	"/usr/sbin/ping",
	"/usr/bin/ping",
	"/sbin/ping",
	"/bin/ping",
	nullptr
} ; /* end array (pings) */

typedef int (suber::*suber_m)() noex ;

constexpr suber_m	mems[] = {
    	&suber::getaddr,
    	&suber::getprog,
    	&suber::getping
} ; /* end array (mems) */

local bufsizevar	rbuflen(bufsize_mn) ;	/* max-name */

cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int inetping(cchar *rhost,int to) noex {
	int		rs = SR_FAULT ;
	CDEBPR("ent host=%s to=%d\n",rhost,to) ;
	if (rhost) ylikely {
	    rs = SR_INVALID ;
	    if (rhost[0]) ylikely {
		suber sub(rhost,to) ;
		rs = sub ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	CDEBPR("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (inetping) */


/* local subroutines */

suber::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = lm_ho(&hebuf)) >= 0) ylikely {
	    helen = rs ;
	    {
		for (cauto &m : mems) {
		    rs = (this->*m)() ;
		    if (rs < 0) break ;
		} /* end for */
	    }
	    rs1 = lm_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (suber::operator) */

int suber::getaddr() noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	if ((a = inet_addr(rhost)) == ADDR_NOT) {
	    HOSTENT	*hep = &he ;
	    if ((rs = getheour(rhost,np,hep,hebuf,helen)) >= 0) {
		if (hep->h_addrtype != AF_INET) {
		    rs = SR_HOSTUNREACH ;
		}
	    } /* end if (getheour) */
	} else {
	    fnumeric = true ;
	} /* end (non-numeric addresses) */
	return rs ;
} /* end method (suber::getaddr) */

int suber::getprog() noex {
    	int		rs = SR_NOENT ;
	for (int i = 0 ; pings[i] != nullptr ; i += 1) {
	    if ((rs = u_access(pings[i],X_OK)) >= 0) {
		prog = pings[i] ;
		break ;
	    }
	} /* end for */
	return rs ;
} /* end method (suber::getprog) */

int suber::getping() noex {
    	int		rs ;
	int		rs1 ;
	if (fnumeric) {
	    rs = pingone(prog,&a,to) ;
	} else {
	    if (hostent_cur hc ; (rs = hostent_curbegin(&he,&hc)) >= 0) {
		cauto enumaddr = hostent_curenumaddr ;
		for (cuchar *ap ; (rs = enumaddr(&he,&hc,&ap)) > 0 ; ) {
		    const in4_addr_t *iap = addrp(ap) ;
		    rs = pingone(prog,iap,to) ;
		    if (rs >= 0) break ;
		} /* end for */
		rs1 = hostent_curend(&he,&hc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (numberic or otherwise) */
	return rs ;
} /* end method (suber::getping) */

local int pingone(cchar *pingprog,const in4_addr_t *ap,int to) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	CDEBPR("ent to=%d\n",to) ;
	if (to < 0) to = TO_PING ;
	inetaddrs at = inetaddr_bin ;
	if (inetaddr ia ; (rs = inetaddr_start(&ia,at,ap,-1)) >= 0) ylikely {
	    cint	dotlen = DOTBUFLEN ;
	    char	dotbuf[DOTBUFLEN + 1] ;
	    if ((rs = inetaddr_getdotaddr(&ia,dotbuf,dotlen)) >= 0) ylikely {
		spawnproc_con	ps{} ;
		cint		tolen = TOBUFLEN ;
		int		ai = 0 ;
		cchar		*args[4] ;
		char		tobuf[TOBUFLEN+1] ;

		args[ai++] = PROGNAME_PING ;
		args[ai++] = dotbuf ;
	        if (to >= 0) {
	    	    ctdeci(tobuf,tolen,to) ;
	    	    args[ai++] = tobuf ;
	        }
		args[ai] = nullptr ;

		ps.disp[0] = SPAWNPROC_DNULL ;
		ps.disp[1] = SPAWNPROC_DCREATE ;
		ps.disp[2] = SPAWNPROC_DNULL ;
		if ((rs = spawnproc(&ps,pingprog,args,np)) >= 0) {
		    const pid_t	pid = rs ;
		    cint	fd = ps.fd[1] ;
		    int		cs = 0 ;
		    if (to < 0) to = TO_PING ;
		    to += TO_MORETIME ;
		    if ((rs = pingoneresp(fd,to)) >= 0) {
	        	rs = u_waitpid(pid,&cs,WNOHANG) ;
	    	    } else {
			rv = rs ;
	        	u_kill(pid,SIGTERM) ;
	        	rs = u_waitpid(pid,&cs,WNOHANG) ;
		    }
		    u_close(fd) ;
		} /* end if (spawnproc) */
		CDEBPR("spawnproc-out rs=%d\n",rs) ;
	    } /* end if (inetaddr_getdotaddr) */
	    rs1 = inetaddr_finish(&ia) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (inetaddr) */
	CDEBPR("ret rs=%d rv=%d\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (pingone) */

local int pingoneresp(int fd,int to) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	CDEBPR("ent\n") ;
	if ((rs = rbuflen) >= 0) ylikely {
	    cint rlen = rs ;
	    if (char *rbuf ; (rs = lm_mall((rlen + 1),&rbuf)) >= 0) ylikely {
	        int	tl = 0 ;
	        int	t = 0 ;
		int	len = 0 ; /* used-multiple */
	        while ((rs >= 0) && (tl < rlen) && (t < to)) {
	            int		f_found{} ; /* used-afterwards */
	            int		rl = (rlen-tl) ;
	            char	*rp = (rbuf+tl) ;
	            if ((rs = uc_readlnto(fd,rp,rl,TO_READ)) >= 0) {
	                len = rs ;
	                if (len > 0) {
	                    CDEBPR("rbuf=>%r<\n",rp,len) ;
	                    f_found = (strnchr(rp,len,'\n') != np) ;
	                    tl += len ;
	                    if (f_found) break ;
	                } else {
	                    t += TO_READ ;
	                }
	                CDEBPR("bottom loop to=%d\n",to) ;
	            } /* end if (uc_readlnto) */
	            if (f_found || (len == 0)) break ;
	        } /* end while (reading response from PING program) */
	        if (rs >= 0) {
	            rs = pingoneparse(rbuf,tl) ;
	        } /* end if (ok) */
		rs1 = lm_free(rbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (rbuflen) */
	CDEBPR("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingoneresp) */

local int pingoneparse(cchar *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		cl ;
	if (cchar *cp ; (cl = sfnext(rbuf,rlen,&cp)) > 0) ylikely {
	    if (sfsub((rbuf+cl),(rlen-cl),"is alive",&cp) < 0) {
	        rs = SR_HOSTDOWN ;
	    }
	} else {
	    rs = SR_HOSTDOWN ;
	}
	return rs ;
} /* end subroutine (pingoneparse) */

#if	CF_DEBUG & CF_DEBUGINT
local int makeint(void *addr) noex {
	int		hi = 0 ;
	uchar		*us = (uchar *) addr ;
	hi |= ((us[3] & 0xFF) << 24) ;
	hi |= ((us[2] & 0xFF) << 16) ;
	hi |= ((us[1] & 0xFF) << 8) ;
	hi |= (us[0] & 0xFF)  ;
	return hi ;
} /* end subroutine (makeint) */
#endif /* CF_DEBUG */


