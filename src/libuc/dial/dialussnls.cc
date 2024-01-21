/* dialussnls SUPPORT */
/* lang=C++20 */

/* dial out to a server listening on UNIX®NLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is the NLS version of the TCP dialer.

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
#include	<ctime>
#include	<usystem.h>
#include	<usupport.h>
#include	<buffer.h>
#include	<char.h>
#include	<localmisc.h>

#include	"nlsdialassist.h"
#include	"dial.h"


/* local defines */

#define	SRV_TCPMUX	"tcpmux"
#define	SRV_LISTEN	"listen"
#define	SRV_TCPNLS	"tcpnls"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif

#define	NLSBUFLEN	(SVCLEN + 30)
#define	RBUFLEN		MAXNAMELEN


/* local namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
    extern int	dialuss(cchar *,int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dialussnls(cchar *portspec,cchar *svcbuf,int to,int aopts) noex {
	cint		nlslen = NLSBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		svclen ;
	int		fd = -1 ;
	char		*nlsbuf ;

	if ((portspec == NULL) || (portspec[0] == '\0'))
	    portspec = NULL ;

/* perform some cleanup on the service code specification */

	if (svcbuf == NULL)
	    return SR_INVAL ;

	while (CHAR_ISWHITE(*svcbuf))
	    svcbuf += 1 ;

	svclen = strlen(svcbuf) ;

	while (svclen && CHAR_ISWHITE(svcbuf[svclen - 1]))
	    svclen -= 1 ;

	if (svclen <= 0)
	    return SR_INVAL ;

/* format the service string to be transmitted */

	if ((rs = uc_malloc((nlslen+1),&nlsbuf)) >= 0) {
	    if ((rs = mknlsreq(nlsbuf,nlslen,svcbuf,svclen)) >= 0) {
	        struct sigaction	osig, nsig ;
	        sigset_t		signalmask ;
	        int			blen = rs ;

	        uc_sigsetempty(&signalmask) ;

	        memclear(&nsig) ;
	        nsig.sa_handler = SIG_IGN ;
	        nsig.sa_mask = signalmask ;
	        nsig.sa_flags = 0 ;
	        if ((rs = u_sigaction(SIGPIPE,&nsig,&osig)) >= 0) {

	            if (portspec == NULL)
	                portspec = "/tmp/unix" ;

	            if ((rs = dialuss(portspec,to,aopts)) >= 0) {
	                fd = rs ;

	                if ((rs = uc_writen(fd,nlsbuf,blen)) >= 0) {
	                    cint	rlen = RBUFLEN ;
	                    char	rbuf[RBUFLEN+1] = { 0 } ;
	                    rs = readnlsresp(fd,rbuf,rlen,to) ;
	                } /* end if (read response) */

	                if (rs < 0) u_close(fd) ;
	            } /* end if (opened) */

	            u_sigaction(SIGPIPE,&osig,NULL) ;
	        } /* end if (sigaction) */

	    } else {
	        rs = SR_TOOBIG ;
	    }
	    rs1 = uc_free(nlsbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialussnls) */


