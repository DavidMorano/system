/* dialticotsordmux SUPPORT */
/* lang=C++20 */

/* dial to the server listening on USSMUX */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	dialticotsordmux

	Description:
	This object dials out to a UNIX®-Socket-Stream (USS) that
	implements a multiplexor on the server side.

	Synopsis:
	int dialticotsordmux(cc *abuf,int alen,cc *svc,mv sargv,
			int to,int opts) noex

	Origin:
	-Dial
	-Transport
	-Interface
	-Connection
	-Oriented
	-Transport
	-Service
	-Orderly
	-Release

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<buffer.h>
#include	<sfx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"dialtixotsord.h"


/* local defines */

#define	DBUFLEN		(8 * 1024)

#ifndef	PORTSPEC_USSMUX
#define	PORTSPEC_USSMUX	"/tmp/ussmux"
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
     extern int	mkquoted(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	dialer(buffer *,cchar *,int,char *,int,int,int) noex ;


/* local variables */

static constexpr int	sigblocks[] = {
	SIGPIPE,
	SIGHUP,
	0
} ;


/* exported subroutines */

int dialticotsordmux(cc *abuf,int alen,cc *svc,mv sargv,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (abuf && svc) {
	    rs = SR_INVALID ;
	    if (svc[0]) {
	        int	sl ;
	        cchar	*sp{} ;
		if ((cl = sfshrink(svc,-1,&sp)) > 0) {
	            buffer	srv ;
	            if ((rs = buffer_start(&srv,100)) >= 0) {
	                cint	dlen = DBUFLEN ;
	                char	*dbuf ;
	                if ((rs = uc_malloc((dlen+1),&dbuf)) >= 0) {
	                    buffer_strw(&srv,svc,svclen) ;
	                    if (sargv != NULL) {
		                auto	mq = mkquoted ;
	                        for (int i = 0 ; sargv[i] ; i += 1) {
				    cchar	*sap = sargv[i] ;
	                            if ((rs = mq(dbuf,dlen,sap,-1)) >= 0) {
	                                buffer_chr(&srv,' ') ;
	                                buffer_buf(&srv,dbuf,rs) ;
	                            } /* end if (mkquoted) */
				    if (rs < 0) break ;
	                        } /* end for */
	                    } /* end if */
	                    if (rs >= 0) {
	                        buffer_chr(&srv,'\n') ;
	                    }
	                    if (rs >= 0) {
	                        rs = dialer(&srv,abuf,alen,dbuf,dlen,to,opts) ;
	                        fd = rs ;
	                    } /* end if (positive) */
	                    rs1 = uc_free(dbuf) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
	                rs1 = buffer_finish(&srv) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (buffer) */
	            if ((rs >= 0) && (fd >= 0)) u_close(fd) ;
		} /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialticotsordmux) */


/* local subroutines */

static int dialer(buffer *sbp,cchar *abuf,int alen,char *dbuf,int dlen,
		int to,int opts) noex {
	sigblocker	ss ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	cchar		*bp ;
	if ((rs = buffer_get(sbp,&bp)) >= 0) {
	    cint	blen = rs ;
	    if ((rs = sigblocker_start(&ss,sigblocks)) >= 0) {
	        if ((rs = dialticotsord(abuf,alen,to,opts)) >= 0) {
	            fd = rs ;
	            if ((rs = uc_writen(fd,bp,blen)) >= 0) {
	                dbuf[0] = '\0' ;
	                if ((rs = uc_readlinetimed(fd,dbuf,dlen,to)) >= 0) {
	                    if ((rs == 0) || (dbuf[0] != '+')) {
	                        rs = SR_BADREQUEST ;
	                    }
	                }
	            } /* end if (wrote service code) */
	        } /* end if (opened) */
	        rs1 = sigblocker_finish(&ss) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs < 0) && (fd >= 0)) {
		u_close(fd) ;
	    }
	} /* end if (buffer_get) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutiner (dialer) */


