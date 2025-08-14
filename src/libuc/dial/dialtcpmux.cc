/* dialtcpmux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial out to a machine server listening on TCPMUX */
/* version %I% last-modified %G% */

#define	CF_CR		1		/* stupid CR character */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialtcpmux

	Description:
	This subroutine will dial out to an INET host using the
	TCPMUX protocol and the optional TCP port and the TCPMUX
	service that is specified.

	Synopsis:
	int dialtcpmux(cc *hn,cc *ps,int af,cc *svc,mainv sargs,
		int to,int opts) noex

	Arguments:
	hn		host to dial to
	ps		port specification to use
	af		address family
	svc		service specification
	sargs		pointer to array of pointers to arguments
	to		to ('>0' means use one, '-1' means do not)
	opts		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	What is up with the 'to' argument?
	>0		use the to as it is
	==0             asynchronously span the connect and do not wait
			for it
	<0              use the system default to (xx minutes --
			whatever)

	Notes:
	Generally, you MUST turn on the compile-time flag at the
	top of this file named 'CF_CR' in order to enable code that
	will transmit a CR character in addition to a normal NL
	character at the end of the service string in the dial
	sequence.  Many stupid TCPMUX servers out there will not
	work otherwise.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<mkx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"dialtcp.h"

import libutil ;

/* local defines */

#ifndef	PORTSPEC_TCPMUX
#define	PORTSPEC_TCPMUX		"tcpmux"
#endif

#ifndef	CF_CR
#define	CF_CR		1		/* stupid CR character */
#endif


/* imported namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct muxhelp {
	cchar		*hn ;
	cchar		*ps ;
	cchar		*sp ;
	mainv		sargs ;
	char		*mbuf = nullptr ;
	int		af ;
	int		sl ;
	int		to ;
	int		dot ;
	int		mlen = 0 ;
	muxhelp(cc *h,cc *p,int f,cc *s,mainv a,int t,int o) noex {
	    hn = h ;
	    ps = p ;
	    af = f ;
	    sl = sfshrink(s,-1,&sp) ;
	    sargs = a ;
	    to = t ;
	    dot = o ;
	} ; /* end ctor */
	int start() noex ;
	int finish() noex ;
	int mkreq() noex ;
	int blocker() noex ;
	int reqsvc(int) noex ;
	~muxhelp() {
	    (void) finish() ;
	} ;
    } ; /* end struct (muxhelp) */
}


/* forward references */

static int	getmuxlen(int,mainv) noex ;
static int	mkmuxreq(char *,int,cchar *,int,mainv) noex ;


/* local variables */

const bool	f_cr = CF_CR ;


/* exported variables */


/* exported subroutines */

int dialtcpmux(cc *hn,cc *ps,int af,cc *svc,mainv sargs,int to,int dot) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if ((ps == nullptr) || (ps[0] == '\0')) ps = PORTSPEC_TCPMUX ;
	if (hn && svc) {
	    rs = SR_INVALID ;
	    if (hn[0] && ps[0] && svc[0]) {
		muxhelp		mo(hn,ps,af,svc,sargs,to,dot) ;
		if ((rs = mo.start()) >= 0) {
		    if ((rs = mo.mkreq()) >= 0) {
			rs = mo.blocker() ;
			fd = rs ;
		    }
		    rs1 = mo.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialtcpmux) */


/* local subroutines */

int muxhelp::start() noex {
	int		rs ;
	mlen = getmuxlen(sl,sargs) ;
	if (void *vp ; (rs = uc_malloc((mlen+1),&vp)) >= 0) {
	    mbuf = charp(vp) ;
	}
	return rs ;
}
/* end method (muxhelp::start) */

int muxhelp::finish() noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
	    rs1 = uc_free(mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    mbuf = nullptr ;
	}
	return rs ;
}
/* end method (muxhelp::finish) */

int muxhelp::mkreq() noex {
	return mkmuxreq(mbuf,mlen,sp,sl,sargs) ;
}
/* end method (muxhelp::mkreq) */

int muxhelp::blocker() noex {
	SIGACTION	sighand{} ;
	SIGACTION	osighand ;
	sigset_t	signalmask ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	uc_sigsetempty(&signalmask) ;
	sighand.sa_handler = SIG_IGN ;
	sighand.sa_mask = signalmask ;
	sighand.sa_flags = 0 ;
	if ((rs = u_sigaction(SIGPIPE,&sighand,&osighand)) >= 0) {
	    if ((rs = dialtcp(hn,ps,af,to,dot)) >= 0) {
		fd = rs ;
		rs = reqsvc(fd) ;
	    } /* end if (opened) */
	    rs1 = u_sigaction(SIGPIPE,&osighand,nullptr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigaction) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end method (muxhelp::blocker) */

int muxhelp::reqsvc(int fd) noex {
	int		rs ;
	int		rs1 ;
        if ((rs = uc_writen(fd,mbuf,mlen)) >= 0) {
            if (char *rbuf  ; (rs = malloc_mn(&rbuf)) >= 0) {
		cint	rlen = rs ;
                if ((rs = uc_readlinetimed(fd,rbuf,rlen,to)) >= 0) {
                    if ((rs == 0) || (rbuf[0] != '+')) {
                        rs = SR_BADREQUEST ;
		    }
                }
		rs1 = uc_free(rbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
        } /* end if (read response) */
	return rs ;
}
/* end method (muxhelp::reqsvc) */

static int getmuxlen(int svclen,mainv sargs) noex {
	int		ml = (svclen+4) ;
	if (sargs) {
	    for (int i = 0 ; sargs[i] ; i += 1) {
		cchar	*sap = sargs[i] ;
		ml += lenstr(sap + 3) ;
	    } /* end for */
	} /* end if */
	return ml ;
}
/* end subroutine (getmuxlen) */

static int mkmuxreq(char *mbuf,int mlen,cc *sp,int sl,mainv sargs) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (sbuf b ; (rs = b.start(mbuf,mlen)) >= 0) {
	    if ((rs = b.strw(sp,sl)) >= 0) {
	        if (sargs) {
	            if (char *qbuf ; (rs = malloc_mn(&qbuf)) >= 0) {
			cint	qlen = rs ;
	                for (int i = 0 ; (rs >= 0) && sargs[i] ; i += 1) {
			    cchar	*sa = sargs[i] ;
	                    b.chr(' ') ;
	                    if ((rs = mkquoted(qbuf,qlen,sa,-1)) >= 0) {
	                        len = rs ;
	                        rs = b.buf(qbuf,len) ;
	                    }
	                } /* end for */
		        rs1 = uc_free(qbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (svc-args) */
	        if (rs >= 0) {
		    if_constexpr (f_cr) {
	                b.chr('\r') ;
		    }
	            b.chr('\n') ;
	        } /* end if */
	    } /* end if */
	    len = b.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkmuxreq) */


