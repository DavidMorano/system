/* simsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* messages for SIMSG requests-responses */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the subroutines to make and parse the
	SIMSG family of messages.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"simsg.h"


/* local defines */


/* external subroutines */


/* local structures */


/* local variables */


/* exported subroutines */

int simsg_sysmiscget(SIMSG_SYSMISCGET *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&msgbuf,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = simsgtype_sysmiscget ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;
	        serialbuf_wui(&msgbuf,sp->tag) ;
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
/* end subroutine (simsg_getsysmisc) */

int simsg_sysmisc(SIMSG_SYSMISC *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&msgbuf,&sp->tag) ;
	        serialbuf_rui(&msgbuf,&sp->la1m) ;
	        serialbuf_rui(&msgbuf,&sp->la5m) ;
	        serialbuf_rui(&msgbuf,&sp->la15m) ;
	        serialbuf_rui(&msgbuf,&sp->btime) ;
	        serialbuf_rui(&msgbuf,&sp->ncpu) ;
	        serialbuf_rui(&msgbuf,&sp->nproc) ;
	        serialbuf_ruc(&msgbuf,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = simsgtype_sysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;
	        serialbuf_wui(&msgbuf,sp->tag) ;
	        serialbuf_wui(&msgbuf,sp->la1m) ;
	        serialbuf_wui(&msgbuf,sp->la5m) ;
	        serialbuf_wui(&msgbuf,sp->la15m) ;
	        serialbuf_wui(&msgbuf,sp->btime) ;
	        serialbuf_wui(&msgbuf,sp->ncpu) ;
	        serialbuf_wui(&msgbuf,sp->nproc) ;
	        serialbuf_wuc(&msgbuf,sp->rc) ;
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
/* end subroutine (simsg_sysmisc) */


