/* getproto SUPPORT */
/* lang=C20 */

/* get a protocol number given a protocol name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	We try to retrieve a protocol number given a protocol name.

	Synopsis:
	int getproto_name(char *pp,int pl) noex

	Arguments:
	pp		pointer to protocol name string
	pl		length of protocol name string

	Returns:
	>=0		protocol number
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<nulstr.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"getproto.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getproto_name(cchar *protop,int protol) noex {
	nulstr		n ;
	int		rs ;
	int		rs1 ;
	int		proto = 0 ;
	cchar		*pname{} ;
	if ((rs = nulstr_start(&n,protop,protol,&pname)) >= 0) {
	    if ((rs = getbufsize(getbufsize_pr)) >= 0) {
	        PROTOENT	pe ;
	        cint		pelen = rs ;
	        char		*pebuf{} ;
	        if ((rs = uc_malloc((pelen+1),&pebuf)) >= 0) {
	            if ((rs = uc_getprotobyname(&pe,pebuf,pelen,pname)) >= 0) {
	                proto = pe.p_proto ;
	            }
	            rs1 = uc_free(pebuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	    } /* end if (getbufsize) */
	    rs1 = nulstr_finish(&n) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return (rs >= 0) ? proto : rs ;
}
/* end subroutine (getproto_name) */


