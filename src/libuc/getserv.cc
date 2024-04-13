/* getserv SUPPORT */
/* lang=C++20 */

/* get a network service number (port) given protocol and service name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getserv_name

	Description:
	Get a service number (a port number really) given a protocol
	name and a service name. Remember that each protocol can
	have its own port associated with any given service name.
	So therefore each service name can have a separate port
	depending on what protocol name it is associated with.

	Synopsis:
	int getserv_name(cchar *protoname,cchar *svc) noex

	Arguments:
	protoname	protocol name
	svc		service name

	Returns:
	>=0		port number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getxx.h>
#include	<localmisc.h>

#include	"getserv.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getserv_name(cchar *pn,cchar *svc) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		port = 0 ;
	if (pn && svc) {
	    rs = SR_INVALID ;
	    if (pn[0] && svc[0]) {
	        char	*svbuf{} ;
	        if ((rs = malloc_sv(&svbuf)) >= 0) {
	            ucentsv	sv ;
		    cint	svlen = rs ;
	            if ((rs = getsv_name(&sv,svbuf,svlen,pn,svc)) >= 0) {
	                port = int(ntohs(sv.s_port)) ;
	            }
	            rs1 = uc_free(svbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (getserv_name) */


