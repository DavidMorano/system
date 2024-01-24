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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<buffer.h>
#include	<sigblocker.h>
#include	<char.h>
#include	<localmisc.h>


/* local defines */

#define	DBUFLEN		(8 * 1024)

#ifndef	PORTSPEC_USSMUX
#define	PORTSPEC_USSMUX	"/tmp/ussmux"
#endif


/* local namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
     extern int	dialticotsord(cchar *,int,int,int) noex ;
     extern int	mkquoted(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	dialer(buffer *,cchar *,int,char *,int,int,int) noex ;


/* local variables */

static constexpr int	sigblocks[] = {
	SIGPIPE,
	0
} ;


/* exported subroutines */

int dialticotsordmux(cc *abuf,int alen,cc *svc,mv sargv,int to,int opts) noex {
	buffer		srvbuf ;
	int		rs ;
	int		rs1 ;
	int		svclen ;
	int		fd = -1 ;

	if (abuf == NULL) return SR_FAULT ;
	if (svc == NULL) return SR_FAULT ;

	if (svc[0] == '\0') return SR_INVALID ;

	while (CHAR_ISWHITE(*svc)) {
	    svc += 1 ;
	}

	svclen = strlen(svc) ;

	while (svclen && CHAR_ISWHITE(svc[svclen - 1])) {
	    svclen -= 1 ;
	}

	if (svclen <= 0)
	    return SR_INVAL ;

/* format the service string to be transmitted */

	if ((rs = buffer_start(&srvbuf,100)) >= 0) {
	    cint	dlen = DBUFLEN ;
	    char	*dbuf ;
	    if ((rs = uc_malloc((dlen+1),&dbuf)) >= 0) {
/* format the service code and arguments for transmission */
	        buffer_strw(&srvbuf,svc,svclen) ;
	        if (sargv != NULL) {
	            int		i ;
	            for (int i = 0 ; (rs >= 0) && sargv[i] ; i += 1) {
	                if ((rs = mkquoted(dbuf,dlen,sargv[i],-1)) >= 0) {
	                    buffer_char(&srvbuf,' ') ;
	                    buffer_buf(&srvbuf,dbuf,rs) ;
	                }
	            } /* end for */
	        } /* end if */
	        if (rs >= 0) {
	            buffer_char(&srvbuf,'\n') ;
	        }
	        if (rs >= 0) {
	            rs = dialer(&srvbuf,abuf,alen,dbuf,dlen,to,opts) ;
	            fd = rs ;
	        } /* end if (positive) */
	        rs1 = uc_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = buffer_finish(&srvbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	if ((rs >= 0) && (fd >= 0)) u_close(fd) ;

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
	            if (rs < 0) {
	                u_close(fd) ;
	                fd = -1 ;
	            }
	        } /* end if (opened) */
	        rs1 = sigblocker_finish(&ss) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (buffer_get) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutiner (dialer) */


