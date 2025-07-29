/* getproto SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a protocol number given a protocol name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getproto_name

	Description:
	We try to retrieve a protocol number given a protocol name.

	Synopsis:
	int getproto_name(char *pp,int pl) noex

	Arguments:
	pp		pointer to protocol name string
	pl		length of protocol name string

	Returns:
	>=0		protocol number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getxx.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"getproto.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getproto_name(cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		proto = 0 ;
	if (pnp) ylikely {
	    rs = SR_INVALID ;
	    if (pnp[0]) ylikely {
	        cchar	*pname{} ;
	        if (nulstr n ; (rs = n.start(pnp,pnl,&pname)) >= 0) {
	    	    if (char *prbuf{} ; (rs = malloc_pr(&prbuf)) >= 0) {
	                ucentpr		pr ;
	                cint		prlen = rs ;
	                if ((rs = uc_getprnam(&pr,prbuf,prlen,pname)) >= 0) {
	                    proto = pr.p_proto ;
	                }
		        rs = rsfree(rs,prbuf) ;
	            } /* end if (m-a) */
	            rs1 = n.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? proto : rs ;
}
/* end subroutine (getproto_name) */


