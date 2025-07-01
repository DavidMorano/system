/* dialussmux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial to the server listening on USSMUX */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object dials out to a UNIX®-Socket-Stream (USS) that
	implements a multiplexor on the server side.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<buffer.h>
#include	<sfx.h>
#include	<mkx.h>			/* |mkquoted(3uc)| */
#include	<char.h>
#include	<localmisc.h>

#include	"dialuss.h"


/* local defines */

#ifndef	PORTSPEC_USSMUX
#define	PORTSPEC_USSMUX	"/tmp/ussmux"
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	dialer(cc *,cc *,int,mv,int,int) noex ;
static int	buffer_loadargs(buffer *,mainv) noex ;
static int	badreq(int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dialussmux(cc *ps,cc *svc,mv sargv,int to,int dot) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (ps && svc) {
	    rs = SR_INVALID ;
	    if (ps[0] && svc[0]) {
		cchar	*sp{} ;
		if (int sl ; (sl = sfshrink(svc,-1,&sp)) > 0) {
		    rs = dialer(ps,sp,sl,sargv,to,dot) ;
		    fd = rs ;
		} /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialussmux) */


/* local subroutines */

static int dialer(cc *ps,cc *sp,int sl,mv sargv,int to,int dot) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (buffer srvbuf ; (rs = buffer_start(&srvbuf,100)) >= 0) {
	    buffer_strw(&srvbuf,sp,sl) ;
	    if ((rs = buffer_loadargs(&srvbuf,sargv)) >= 0) {
		cchar	*bp{} ;
	        buffer_chr(&srvbuf,'\n') ;
	        if ((rs = buffer_get(&srvbuf,&bp)) >= 0) {
		    SIGACTION	osigs ;
		    SIGACTION	sigs{} ;
		    sigset_t	signalmask ;
	            int		blen = rs ;
	            uc_sigsetempty(&signalmask) ;
	            sigs.sa_handler = SIG_IGN ;
	            sigs.sa_mask = signalmask ;
	            sigs.sa_flags = 0 ;
	            if ((rs = u_sigaction(SIGPIPE,&sigs,&osigs)) >= 0) {
	                if ((rs = dialuss(ps,to,dot)) >= 0) {
	                    fd = rs ;
	                    if ((rs = uc_writen(fd,bp,blen)) >= 0) {
	                        auto	rln = uc_readlinetimed ;
	                        if (char *rbuf ; (rs = malloc_mn(&rbuf)) >= 0) {
	                            cint	rlen = rs ;
	                            if ((rs = rln(fd,rbuf,rlen,to)) >= 0) {
					rs = badreq(rs,rbuf) ;
	                            } /* end if (rln) */
				    rs1 = uc_free(rbuf) ;
				    if (rs >= 0) rs = rs1 ;
				} /* end if (m-a-f) */
	                    } /* end if (wrote service code) */
	                } /* end if (dialuss) */
	                rs1 = u_sigaction(SIGPIPE,&osigs,nullptr) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (sigs) */
		} /* end if (buffer_get) */
	    } else {
	        rs = SR_TOOBIG ;
	    }
	    rs1 = buffer_finish(&srvbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) {
		u_close(fd) ;
	    }
	} /* end if (buffer) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialer) */

static int buffer_loadargs(buffer *bp,mainv sargv) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sargv != nullptr) {
	    if (char *qbuf ; (rs = malloc_ml(&qbuf)) >= 0) {
	        cint	qlen = rs ;
	        for (int i = 0 ; sargv[i] ; i += 1) {
	            rs = mkquoted(qbuf,qlen,sargv[i],-1) ;
	            if (rs < 0) break ;
	            bp->chr(' ') ;
	            bp->buf(qbuf,rs) ;
	        } /* end for */
		rs1 = uc_free(qbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_loadargs) */

static int badreq(int rs,cchar *rbuf) noex {
	if ((rs == 0) || (rbuf[0] != '+')) {
	    rs = SR_BADREQUEST ;
	}
	return rs ;
}
/* end subroutine (badreq) */


