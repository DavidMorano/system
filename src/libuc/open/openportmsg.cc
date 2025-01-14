/* openportmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* message for |openport(3dam)| support IPC */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This module contains the code to make and parse the internal
        messages to support |openport(3dam)| IPC.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<usystem.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"openportmsg.h"


/* local defines */

#define	OPM_REQ		struct openportmsg_request
#define	OPM_RES		struct openportmsg_response


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* local variables */

constexpr int		unlen = OPENPORTMSG_UNLEN ;


/* exported variables */


/* exported subroutines */

int openportmsg_request(OPM_REQ *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr{} ;
	    int		sz ;
	    uchar	*ubp = (uchar *) &sp->sa ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_ri(&msgbuf,&sp->pf) ;

	        serialbuf_ri(&msgbuf,&sp->ptype) ;

	        serialbuf_ri(&msgbuf,&sp->proto) ;

	        sz = szof(SOCKADDRESS_IN6) ;
	        serialbuf_rubuf(&msgbuf,ubp,sz) ;

	        serialbuf_rstrw(&msgbuf,sp->username,unlen) ;

	    } else { /* write */

	        sp->msgtype = openportmsgtype_request ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wi(&msgbuf,sp->pf) ;

	        serialbuf_wi(&msgbuf,sp->ptype) ;

	        serialbuf_wi(&msgbuf,sp->proto) ;

	        sz = szof(SOCKADDRESS_IN6) ;
	        serialbuf_wubuf(&msgbuf,ubp,sz) ;

	        serialbuf_wstrw(&msgbuf,sp->username,unlen) ;

	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (openportmsg_request) */

/* general response message */
int openportmsg_response(OPM_RES *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr{} ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_ri(&msgbuf,&sp->rs) ;

	    } else { /* write */

	        sp->msgtype = openportmsgtype_response ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wi(&msgbuf,sp->rs) ;

	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (openportmsg_response) */


