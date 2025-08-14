/* dialtcpnls SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial out to a machine server listening on TCPNLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This code was originally written.

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
	to		to ('>0' means use one, '-1' means do not)
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
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sbuf.h>
#include	<char.h>
#include	<sfx.h>
#include	<nlsdialassist.h>
#include	<localmisc.h>

#include	"dialtcp.h"


/* local defines */

#define	SVC_TCPMUX	"tcpmux"
#define	SVC_LISTEN	"listen"
#define	SVC_TCPNLS	"tcpnls"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	dialconn(int,cchar *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dialtcpnls(cc *hn,cc *ps,int af,cc *svc,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (hn && svc) {
	    rs = SR_INVALID ;
	    if (hn[0] && svc[0]) {
	        cchar	*sp{} ;
	        if (int sl ; (sl = sfshrink(svc,-1,&sp)) > 0) {
	            if (char *nlsbuf ; (rs = malloc_mn(&nlsbuf)) >= 0) {
	                cint	nlslen = rs ;
	                if ((rs = mknlsreq(nlsbuf,nlslen,sp,sl)) >= 0) {
		            SIGACTION	osig ;
		            SIGACTION	nsig{} ;
		            sigset_t	sigmask ;
			    cint	sig = SIGPIPE ;
	                    cint	blen = rs ;
	                    uc_sigsetempty(&sigmask) ;
	                    nsig.sa_handler = SIG_IGN ;
	                    nsig.sa_mask = sigmask ;
	                    nsig.sa_flags = 0 ;
	                    if ((rs = u_sigaction(sig,&nsig,&osig)) >= 0) {
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
				    rs = dialconn(fd,nlsbuf,blen,to) ;
	                        } /* end if (opened) */
	                        rs1 = u_sigaction(sig,&osig,nullptr) ;
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


/* local subroutines */

static int dialconn(int fd,cc *nbuf,int nlen,int to) noex {
	int		rs ;
	int		rs1 ;
	if (char *rbuf ; (rs = malloc_mn(&rbuf)) >= 0) {
	    cint	rlen = rs ;
	    if ((rs = uc_writen(fd,nbuf,nlen)) >= 0) {
		rs = readnlsresp(fd,rbuf,rlen,to) ;
	    } /* end if (read response) */
	    rs1 = uc_free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (dialconn) */


