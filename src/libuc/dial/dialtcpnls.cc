/* dialtcpnls SUPPORT */
/* lang=C++20 */

/* dial out to a machine server listening on TCPNLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialtcpnls

	Description:
	This subroutine will dial out to an INET host using the
	TCPMUX protocol and the optional TCP port and TCPMUX services
	that is specified.

	Synopsis:
	int dialtcpnls(cc *hn,cc *ps,int af,cc *svc,int to,int dot) noex

	Arguments:
	hn		host to dial to
	ps		port specification to use
	af		address family
	svc		service specification
	to		to ('>0' means use one, '-1' means don't)
	dot		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	What is up with the 'to' argument?
	>0		use the to as it is
	==0             asynchronously span the connect and do not wait
			for it
	<0              use the system default to (xx minutes --
			whatever)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<sbuf.h>
#include	<char.h>
#include	<sfx.h>
#include	<dialtcp.h>
#include	<localmisc.h>

#include	"nlsdialassist.h"
#include	"dialtcpnls.h"


/* local defines */

#define	SVC_TCPMUX	"tcpmux"
#define	SVC_LISTEN	"listen"
#define	SVC_TCPNLS	"tcpnls"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif

#define	NLSBUFLEN	(SVCLEN + 30)
#define	RBUFLEN		MAXNAMELEN


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dialtcpnls(cc *hn,cc *ps,int af,cc *svc,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (hn && svc) {
	    rs = SR_INVALID ;
	    if (hn[0] && svc[0]) {
	        int		sl ;
	        cchar		*sp ;
	        if ((sl = sfshrink(svc,-1,&sp)) > 0) {
	            cint	nlslen = NLSBUFLEN ;
	            char	*nlsbuf{} ;
	            if ((rs = uc_malloc((nlslen+1),&nlsbuf)) >= 0) {
	                if ((rs = mknlsreq(nlsbuf,nlslen,sp,sl)) >= 0) {
		            SIGACTION	osig ;
		            SIGACTION	nsig{} ;
		            sigset_t	signalmask ;
	                    cint	blen = rs ;
	                    uc_sigsetempty(&signalmask) ;
	                    nsig.sa_handler = SIG_IGN ;
	                    nsig.sa_mask = signalmask ;
	                    nsig.sa_flags = 0 ;
	                    if ((rs = u_sigaction(SIGPIPE,&nsig,&osig)) >= 0) {
	                        if (ps == nullptr) {
	                            ps = SVC_LISTEN ;
	                            rs = dialtcp(hn,ps,af,to,opts) ;
	                            fd = rs ;
	                            if ((rs < 0) && (rs != SR_CONNREFUSED)) {
	                                ps = SVC_TCPNLS ;
	                                rs = dialtcp(hn,ps,af,to,opts) ;
	                                fd = rs ;
	                            } /* end if */
	                        } else {
	                            rs = dialtcp(hn,ps,af,to,opts) ;
	                            fd = rs ;
	                        }
	                        if (rs >= 0) {
	                            if ((rs = uc_writen(fd,nlsbuf,blen)) >= 0) {
	                                cint	rlen = RBUFLEN ;
	                                char	rbuf[RBUFLEN+1] = { 0 } ;
	                                rs = readnlsresp(fd,rbuf,rlen,to) ;
	                            } /* end if (read response) */
	                        } /* end if (opened) */
	                        rs1 = u_sigaction(SIGPIPE,&osig,nullptr) ;
		                if (rs >= 0) rs = rs1 ;
	                    } /* end if (sigaction) */
	                } else {
	                    rs = SR_TOOBIG ;
	                }
	                rs1 = uc_free(nlsbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (memory-allocation) */
	            if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	        } /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialtcpnls) */


