/* mcmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create and parse mail-cluster IPC messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Group:
	mcmsg

	Description:
	This module contains the subroutines to make and parse the
	MCMSG family of messages.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climit>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecstr.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"mcmsg.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mcmsg_request(mcmsg_req *sp,int f,char *buf,int buflen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&msgbuf,buf,buflen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_ruc(&msgbuf,&sp->seq) ;
	        serialbuf_rui(&msgbuf,&sp->tag) ;
	        serialbuf_rui(&msgbuf,&sp->timestamp) ;
	        serialbuf_rstrw(&msgbuf,sp->cluster,MCMSG_LCLUSTER) ;
	        serialbuf_rstrw(&msgbuf,sp->mailuser,MCMSG_LMAILUSER) ;
	    } else { /* write */
	        sp->msgtype = mcmsgtype_request ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;
	        serialbuf_wuc(&msgbuf,sp->seq) ;
	        serialbuf_wui(&msgbuf,sp->tag) ;
	        serialbuf_wui(&msgbuf,sp->timestamp) ;
	        serialbuf_wstrw(&msgbuf,sp->cluster,MCMSG_LCLUSTER) ;
	        serialbuf_wstrw(&msgbuf,sp->mailuser,MCMSG_LMAILUSER) ;
	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(buf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_request) */

int mcmsg_report(mcmsg_rep *sp,int f,char *buf,int buflen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(buf,buflen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        mb.rui(&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        mb.ruc(&sp->seq) ;
	        mb.rui(&sp->tag) ;
	        mb.rui(&sp->timestamp) ;
	        mb.rui(&sp->mlen) ;
	        mb.rstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.rstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	        mb.rstrw(sp->msgid,MCMSG_LMSGID) ;
	        mb.rstrw(sp->from,MCMSG_LFROM) ;
	        mb.ruc(&sp->flags) ;
	        mb.ruc(&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mcmsgtype_report ;
	        hdr = sp->msgtype ;
	        mb.wui(hdr) ;
	        mb.wuc(sp->seq) ;
	        mb.wui(sp->tag) ;
	        mb.wui(sp->timestamp) ;
	        mb.wui(sp->mlen) ;
	        mb.wstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.wstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	        mb.wstrw(sp->msgid,MCMSG_LMSGID) ;
	        mb.wstrw(sp->from,MCMSG_LFROM) ;
	        mb.wuc(sp->flags) ;
	        mb.wuc(sp->rc) ;
	        if ((sp->msglen = mb.getlen) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(buf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_report) */

int mcmsg_acknowledge(mcmsg_ack *sp,int f,char *buf,int buflen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(buf,buflen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        mb.rui(&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        mb.ruc(&sp->seq) ;
	        mb.rui(&sp->tag) ;
	        mb.ruc(&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mcmsgtype_ack ;
	        hdr = sp->msgtype ;
	        mb.wui(hdr) ;
	        mb.wuc(sp->seq) ;
	        mb.wui(sp->tag) ;
	        mb.wuc(sp->rc) ;
	        if ((sp->msglen = mb.getlen) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(buf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_acknowledge) */


