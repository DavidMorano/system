/* dialticotsordnls SUPPORT */
/* lang=C++20 */

/* dial out to a server listening on TI-COTS-ORD-NLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialticotsordnls

	Description:
	This subroutine will dial out to the TICOTSORD transport which
	has the NLS listener on it.

	Synopsis:
	int dialticotsordnls(abuf,alen,svcbuf,to,opts)
	const char	abuf[] ;
	const char	svcbuf[] ;
	int		alen ;
	int		to ;
	int		opts ;

	Arguments:
	abuf		XTI address
	alen		address of XTI address
	svcbuf		service specification
	to		to ('>=0' mean use one, '-1' means don't)
	opts		any dial opts

	Returns:
	>=0		file descriptor
	<0		error in dialing

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<sbuf.h>
#include	<char.h>
#include	<sigblock.h>
#include	<localmisc.h>

#include	"nlsdialassist.h"


/* local defines */

#define	SRV_TCPMUX	"tcpmux"
#define	SRV_LISTEN	"listen"
#define	SRV_TCPNLS	"tcpnls"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif

#define	NLSBUFLEN	(SVCLEN + 30)
#define	NLSBUFLEN	(SVCLEN + 30)

#define	TBUFLEN		MAXNAMELEN

#define	HEXBUFLEN	((2 * MAXPATHLEN) + 2)


/* external subroutines */

extern int	snwcpy(char *,int,const char *,int) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	dialticotsord(const char *,int,int,int) ;

extern char	*strnchr(const char *,int,int) ;


/* external variables */


/* local structures */


/* forward references */


/* local vaiables */


/* exported subroutines */


int dialticotsordnls(abuf,alen,svcbuf,to,opts)
const char	abuf[] ;
const char	svcbuf[] ;
int		alen ;
int		to ;
int		opts ;
{
	SIGACTION	sigs, oldsigs ;
	sigset_t	signalmask ;
	cint	nlslen = NLSBUFLEN ;
	int		rs = SR_OK ;
	int		svclen ;
	int		fd = -1 ;
	char		nlsbuf[NLSBUFLEN + 1] ;

	if ((abuf != NULL) && (abuf[0] == '\0'))
	    abuf = NULL ;

/* perform some cleanup on the service code specification */

	if (svcbuf == NULL)
	    return SR_INVAL ;

	while (CHAR_ISWHITE(*svcbuf)) svcbuf += 1 ;
	svclen = strlen(svcbuf) ;

	while (svclen && CHAR_ISWHITE(svcbuf[svclen - 1])) {
	    svclen -= 1 ;
	}

	if (svclen <= 0)
	    return SR_INVAL ;

	if (abuf == NULL) {
	    abuf = "local" ;
	    alen = strlen(abuf) ;
	} /* end if (default UNIX® address!) */

	if ((rs = mknlsreq(nlsbuf,nlslen,svcbuf,svclen)) >= 0) {
	    cint	blen = rs ;

	    uc_sigsetempty(&signalmask) ;

	    memclear(&sigs) ;
	    sigs.sa_handler = SIG_IGN ;
	    sigs.sa_mask = signalmask ;
	    sigs.sa_flags = 0 ;
	    if ((rs = u_sigaction(SIGPIPE,&sigs,&oldsigs)) >= 0) {

	        if ((rs = dialticotsord(abuf,alen,to,opts)) >= 0) {
	            fd = rs ;

/* transmit the formatted service code and arguments */

	            rs = uc_writen(fd,nlsbuf,blen) ;
	            if (rs >= 0) {
	                cint	tlen = TBUFLEN ;
	                char	tbuf[TBUFLEN+1] ;
	                rs = readnlsresp(fd,tbuf,tlen,to) ;
	            } /* end if (reading response) */

	            if (rs < 0) u_close(fd) ;
	        } /* end if (opened) */

	        u_sigaction(SIGPIPE,&oldsigs,NULL) ;
	    } /* end if (sig-action) */

	} else {
	    rs = SR_TOOBIG ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialticotsordnls) */


