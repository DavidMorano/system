/* dialticotsordnls SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial out to a server listening on TI-COTS-ORD-NLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialticotsordnls

	Description:
	This subroutine will dial out to the TICOTSORD transport which
	has the NLS listener on it.

	Synopsis:
	int dialticotsordnls(cc *abuf,int alen,cc *svc,int to,int dot) noex

	Arguments:
	abuf		XTI address
	alen		address of XTI address
	svcbuf		service specification
	to		to ('>=0' mean use one, '-1' means do not)
	dot

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sbuf.h>
#include	<char.h>
#include	<sigblocker.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"nlsdialassist.h"
#include	"dialtocotsord.h"


/* local defines */

#define	SRV_TCPMUX	"tcpmux"
#define	SRV_LISTEN	"listen"
#define	SRV_TCPNLS	"tcpnls"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif

#define	NLSBUFLEN	(SVCLEN + 30)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local vaiables */


/* exported variables */


/* exported subroutines */

int dialticotsordnls(cc *abuf,int alen,cc *svcbuf,int to,int opts) noex {
	cint		nlslen = NLSBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		svclen ;
	int		fd = -1 ;
	char		nlsbuf[NLSBUFLEN + 1] ;

	if ((abuf != nullptr) && (abuf[0] == '\0'))
	    abuf = nullptr ;

/* perform some cleanup on the service code specification */

	if (svcbuf == nullptr)
	    return SR_INVAL ;

	while (CHAR_ISWHITE(*svcbuf)) svcbuf += 1 ;
	svclen = strlen(svcbuf) ;

	while (svclen && CHAR_ISWHITE(svcbuf[svclen - 1])) {
	    svclen -= 1 ;
	}

	if (svclen <= 0)
	    return SR_INVAL ;

	if (abuf == nullptr) {
	    abuf = "local" ;
	    alen = strlen(abuf) ;
	} /* end if (default UNIX® address!) */

	if ((rs = mknlsreq(nlsbuf,nlslen,svcbuf,svclen)) >= 0) {
	    SIGACTION	osigs ;
	    SIGACTION	nsigs{} ;
	    nsigset_t	sigmask ;
	    cint	blen = rs ;
	    uc_nsigsetempty(&sigmask) ;
	    nsigs.sa_handler = SIG_IGN ;
	    nsigs.sa_mask = sigmask ;
	    nsigs.sa_flags = 0 ;
	    if ((rs = u_sigaction(SIGPIPE,&nsigs,&osigs)) >= 0) {
	        if ((rs = dialticotsord(abuf,alen,to,opts)) >= 0) {
	            fd = rs ;
	            if ((rs = uc_writen(fd,nlsbuf,blen)) >= 0) {
			if (char *tbuf ; (rs = malloc_mn(&tbuf)) >= 0) {
			    cint	tlen = rs ;
			    {
	                        rs = readnlsresp(fd,tbuf,tlen,to) ;
			    }
			    rs = rsfree(rs,tbuf) ;
			} /* end if (m-a-f) *?
	            } /* end if (reading response) */
	        } /* end if (opened) */
	        rs1 = u_sigaction(SIGPIPE,&osigs,nullptr) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sig-action) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} else {
	    rs = SR_TOOBIG ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialticotsordnls) */


