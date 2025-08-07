/* openportmsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message for |openport(3dam)| support IPC */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	openportmsg

	Description:
        This module contains the code to make and parse the internal
        messages to support |openport(3dam)| IPC.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"openportmsg.h"


/* local defines */

#define	OPM_REQ		openportmsg_request
#define	OPM_RES		openportmsg_response


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		unlen = OPENPORTMSG_UNLEN ;


/* exported variables */


/* exported subroutines */

int openportmsg_msgrequest(OPM_REQ *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	    uint	hdr{} ;
	    int		sz ;
	    uchar	*ubp = (uchar *) &sp->sa ;
	    if (f) { /* read */
	        sp->msgtype = openportmsgtype_request ;
	        hdr = sp->msgtype ;
	        sb.wui(hdr) ;
	        sb.wi(sp->pf) ;
	        sb.wi(sp->ptype) ;
	        sb.wi(sp->proto) ;
	        sz = szof(sockaddress) ;
	        sb.wubuf(ubp,sz) ;
	        sb.wstrw(sp->username,unlen) ;
	        if ((sp->msglen = sb.getlen) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } else { /* write */
	        sb.rui(&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        sb.ri(&sp->pf) ;
	        sb.ri(&sp->ptype) ;
	        sb.ri(&sp->proto) ;
	        sz = szof(sockaddress) ;
	        sb.rubuf(ubp,sz) ;
	        sb.rstrw(sp->username,unlen) ;
	    } /* end if */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (openportmsg_msgrequest) */

/* general response message */
int openportmsg_msgresponse(OPM_RES *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	    uint	hdr{} ;
	    if (f) { /* read */
	        sp->msgtype = openportmsgtype_response ;
	        hdr = sp->msgtype ;
	        sb.wui(hdr) ;
	        sb.wi(sp->rs) ;
	        if ((sp->msglen = sb.getlen) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } else { /* write */
	        sb.rui(&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        sb.ri(&sp->rs) ;
	    } /* end if */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (openportmsg_msgresponse) */


