/* dialussnls SUPPORT */
/* lang=C++20 */

/* dial out to a server listening on UNIX® LISTEN */
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
#include	<csignal>
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sfx.h>
#include	<nlsdialassist.h>
#include	<localmisc.h>

#include	"dialuss.h"


/* local defines */

#ifndef	PORTSPEC_USSNLS
#define	PORTSPEC_USSNLS	"/tmp/ussnls"
#endif


/* local namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dialussnls(cchar *portspec,cchar *svc,int to,int aopts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (portspec == nullptr) portspec = PORTSPEC_USSNLS ;
	if (svc) {
	    rs = SR_INVALID ;
	    if (portspec[0] && svc[0]) {
		int	sl ;
		cchar	*sp{} ;
	        if ((sl = sfshrink(svc,-1,&sp)) > 0) {
	            char	*nlsbuf{} ;
	            if ((rs = malloc_mn(&nlsbuf)) >= 0) {
	                cint	nlslen = rs ;
	                if ((rs = mknlsreq(nlsbuf,nlslen,sp,sl)) >= 0) {
	                    SIGACTION	osig ;
	                    SIGACTION	nsig{} ;
	                    sigset_t	sigmask ;
	                    cint	blen = rs ;
	                    uc_sigsetempty(&sigmask) ;
	                    nsig.sa_handler = SIG_IGN ;
	                    nsig.sa_mask = sigmask ;
	                    nsig.sa_flags = 0 ;
	                    if ((rs = u_sigaction(SIGPIPE,&nsig,&osig)) >= 0) {
	                        if ((rs = dialuss(portspec,to,aopts)) >= 0) {
	                            fd = rs ;
	                            if ((rs = uc_writen(fd,nlsbuf,blen)) >= 0) {
	                                char	*rbuf{} ;
					if ((rs = malloc_mn(&rbuf)) >= 0) {
	                                    cint	rlen = rs ;
	                                    rs = readnlsresp(fd,rbuf,rlen,to) ;
					}
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
	            } /* end if (m-a-f) */
		    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	        } /* end if (sfshrink) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialussnls) */


