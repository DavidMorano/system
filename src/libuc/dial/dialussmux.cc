/* dialussmux SUPPORT */
/* lang=C++20 */

/* dial to the server listening on USSMUX */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object dials out to a UNIX�-Socket-Stream (USS) that
	implements a multiplexor on the server side.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<time.h>
#include	<usystem.h>
#include	<buffer.h>
#include	<char.h>
#include	<localmisc.h>

#include	"dial.h"


/* local defines */

#define	QBUFLEN		(MAXNAMELEN +20)
#define	RBUFLEN		LINEBUFLEN

#ifndef	PORTSPEC_USSMUX
#define	PORTSPEC_USSMUX	"/tmp/ussmux"
#endif


/* local namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
    extern int	mkquoted(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local subroutines */


/* exported subroutines */

int dialussmux(cc *portspec,cc *svcspec,mv srvargv,int to,int opts) noex {
	buffer		srvbuf ;
	int		rs ;
	int		rs1 ;
	int		svclen ;
	int		fd = -1 ;
	cchar	*bp ;

	if (portspec == NULL) return SR_FAULT ;
	if (svcspec == NULL) return SR_FAULT ;

	if (portspec[0] == '\0') return SR_INVALID ;
	if (svcspec[0] == '\0') return SR_INVALID ;

	while (CHAR_ISWHITE(*svcspec))
	    svcspec += 1 ;

	svclen = strlen(svcspec) ;

	while (svclen && CHAR_ISWHITE(svcspec[svclen - 1]))
	    svclen -= 1 ;

	if (svclen <= 0)
	    return SR_INVAL ;

/* format the service string to be transmitted */

	if ((rs = buffer_start(&srvbuf,100)) >= 0) {

/* format the service code and arguments for transmission */

	    buffer_strw(&srvbuf,svcspec,svclen) ;

	    if (srvargv != NULL) {
	        const int	qlen = QBUFLEN ;
		int		i ;
	        char		qbuf[QBUFLEN+1] ;
	        for (i = 0 ; srvargv[i] != NULL ; i += 1) {
	            rs = mkquoted(qbuf,qlen,srvargv[i],-1) ;
	            if (rs < 0) break ;
	            buffer_char(&srvbuf,' ') ;
	            buffer_buf(&srvbuf,qbuf,rs) ;
	        } /* end for */
	    } /* end if */

	    if (rs >= 0) {
	        buffer_char(&srvbuf,'\n') ;
	        if ((rs = buffer_get(&srvbuf,&bp)) >= 0) {
		    SIGACTION	sigs, osigs ;
		    sigset_t	signalmask ;
	            int		blen = rs ;
	            uc_sigsetempty(&signalmask) ;
	            memset(&sigs,0,sizeof(SIGACTION)) ;
	            sigs.sa_handler = SIG_IGN ;
	            sigs.sa_mask = signalmask ;
	            sigs.sa_flags = 0 ;
	            if ((rs = u_sigaction(SIGPIPE,&sigs,&osigs)) >= 0) {

	            if ((rs = dialuss(portspec,to,opts)) >= 0) {
	                fd = rs ;

/* transmit the formatted service code and arguments */

	                if ((rs = uc_writen(fd,bp,blen)) >= 0) {
	                    const int	rlen = RBUFLEN ;
	                    char	rbuf[RBUFLEN+1] = { 0 } ;

	                    if ((rs = uc_readlinetimed(fd,rbuf,rlen,to)) >= 0) {
	                        if ((rs == 0) || (rbuf[0] != '+')) {
	                            rs = SR_BADREQUEST ;
				}
	                    }

	                } /* end if (wrote service code) */

	                if (rs < 0) {
	                    u_close(fd) ;
			    fd = -1 ;
			}
	            } /* end if (dialuss) */

	            u_sigaction(SIGPIPE,&osigs,NULL) ;
	        } /* end if (sigs) */
		} /* end if (buffer_get) */
	    } else {
	        rs = SR_TOOBIG ;
	    }

	    rs1 = buffer_finish(&srvbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) {
		u_close(fd) ;
		fd = -1 ;
	    }
	} /* end if (buffer) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialussmux) */


