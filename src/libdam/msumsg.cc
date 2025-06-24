/* msumsg SUPPORT */
/* lang=C++20 (conformance reviewed) */
/* charset=ISO8859-1 */

/* messages for MSU requests-responses */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	msumsg

	Description:
	This module contains the subroutines to make and parse the
	MSUMSG family of messages.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"msumsg.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int msumsg_getstatus(struct msumsg_getstatus *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_getstatus ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_getstatus) */


int msumsg_status(struct msumsg_status *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	        serialbuf_rui(&m,&sp->pid) ;

	        serialbuf_ruc(&m,&sp->rc) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_status ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        serialbuf_wui(&m,sp->pid) ;

	        serialbuf_wuc(&m,sp->rc) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_status) */


int msumsg_getsysmisc(struct msumsg_getsysmisc *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_getsysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_getsysmisc) */


int msumsg_sysmisc(struct msumsg_sysmisc *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	        serialbuf_rui(&m,&sp->pid) ;

	        serialbuf_rui(&m,&sp->utime) ;

	        serialbuf_rui(&m,&sp->btime) ;

	        serialbuf_rui(&m,&sp->ncpu) ;

	        serialbuf_rui(&m,&sp->nproc) ;

	        serialbuf_ruia(&m,sp->la,3) ;

	        serialbuf_ruc(&m,&sp->rc) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_sysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        serialbuf_wui(&m,sp->pid) ;

	        serialbuf_wui(&m,sp->utime) ;

	        serialbuf_wui(&m,sp->btime) ;

	        serialbuf_wui(&m,sp->ncpu) ;

	        serialbuf_wui(&m,sp->nproc) ;

	        serialbuf_wuia(&m,sp->la,3) ;

	        serialbuf_wuc(&m,sp->rc) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_sysmisc) */


int msumsg_exit(struct msumsg_exit *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_exit ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_exit) */


int msumsg_mark(struct msumsg_mark *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_mark ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_mark) */


int msumsg_report(struct msumsg_report *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	m ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&m,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&m,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&m,&sp->tag) ;

	    } else { /* write */

	        sp->msgtype = msumsgtype_report ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;

	        serialbuf_wui(&m,sp->tag) ;

	        if ((sp->msglen = serialbuf_getlen(&m)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msumsg_report) */


