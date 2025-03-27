/* dialcprogmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	dialcprogmsg

	Description:
	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"dialcprogmsg.h"


/* local defines */

#define	DMSG_END	DIALCPROGMSG_END
#define	DMSG_LIGHT	DIALCPROGMSG_LIGHT


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dialcprogmsg_end(char *mbuf,int mlen,bool f,DMSG_END *sp) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	    if (f) { /* read (from object) */
	        sp->type = dialcprogmsgtype_end ;
	        sb << sp->type ;
	        sb << ushort(4) ;
	        sb << sp->flags ;
	        sb << sp->opts ;
	    } else { /* write (to object) */
	        sb >> sp->type ;
	        sb >> sp->len ;
	        sb >> sp->flags ;
	        sb >> sp->opts ;
	    } /* end if */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (dialcprogmsg_end) */

int dialcprogmsg_light(char *mbuf,int mlen,bool f,DMSG_LIGHT *sp) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	    ushort	usw ;
	    if (f) { /* read (from object) */
	        sp->type = dialcprogmsgtype_light ;
	        sb << sp->type ;
	        usw = sp->salen1 + sp->salen2 + (2 * szof(ushort)) ;
	        sb << usw ;
	        sb << sp->salen1 ;
	        sb << sp->salen2 ;
	        sb.wobj(&sp->saout,int(sp->salen1)) ;
	        sb.wobj(&sp->saerr,int(sp->salen2)) ;
	    } else { /* write (to object) */
	        sb >> sp->type ;
	        sb >> sp->len ;
	        sb >> sp->salen1 ;
	        sb >> sp->salen2 ;
	        sb.robj(&sp->saout,int(sp->salen1)) ;
	        sb.robj(&sp->saerr,int(sp->salen2)) ;
	    } /* end if */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (dialcprogmsg_light) */


