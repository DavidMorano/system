/* mcmsg SUPPORT */
/* lang=C++20 */

/* create and parse mail-cluster IPC messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This module contains the subroutines to make and parse the
	MCMSG family of messages.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/uio.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"mcmsg.h"


/* local defines */


/* external subroutines */


/* local structures */


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
	        serialbuf_rui(&msgbuf,&sp->mlen) ;
	        serialbuf_rstrw(&msgbuf,sp->cluster,MCMSG_LCLUSTER) ;
	        serialbuf_rstrw(&msgbuf,sp->mailuser,MCMSG_LMAILUSER) ;
	        serialbuf_rstrw(&msgbuf,sp->msgid,MCMSG_LMSGID) ;
	        serialbuf_rstrw(&msgbuf,sp->from,MCMSG_LFROM) ;
	        serialbuf_ruc(&msgbuf,&sp->flags) ;
	        serialbuf_ruc(&msgbuf,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mcmsgtype_report ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;
	        serialbuf_wuc(&msgbuf,sp->seq) ;
	        serialbuf_wui(&msgbuf,sp->tag) ;
	        serialbuf_wui(&msgbuf,sp->timestamp) ;
	        serialbuf_wui(&msgbuf,sp->mlen) ;
	        serialbuf_wstrw(&msgbuf,sp->cluster,MCMSG_LCLUSTER) ;
	        serialbuf_wstrw(&msgbuf,sp->mailuser,MCMSG_LMAILUSER) ;
	        serialbuf_wstrw(&msgbuf,sp->msgid,MCMSG_LMSGID) ;
	        serialbuf_wstrw(&msgbuf,sp->from,MCMSG_LFROM) ;
	        serialbuf_wuc(&msgbuf,sp->flags) ;
	        serialbuf_wuc(&msgbuf,sp->rc) ;
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
/* end subroutine (mcmsg_report) */

int mcmsg_acknowledge(mcmsg_ack *sp,int f,char *buf,int buflen) noex {
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
	        serialbuf_ruc(&msgbuf,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mcmsgtype_ack ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;
	        serialbuf_wuc(&msgbuf,sp->seq) ;
	        serialbuf_wui(&msgbuf,sp->tag) ;
	        serialbuf_wuc(&msgbuf,sp->rc) ;
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
/* end subroutine (mcmsg_acknowledge) */


