/* mfs-msg */

/* messages for MFS requests-responses */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 2004-07-21, David A�D� Morano
	This module was originally written.

	= 2017-08-10, David A�D� Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright � 2004,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the subroutines to make and parse the MFSMSG
	family of messages.


*******************************************************************************/


#define	MFSMSG_MASTER	0


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>

#include	<usystem.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"mfsmsg.h"


/* local defines */


/* external subroutines */


/* local structures */


/* local variables */


/* exported subroutines */


int mfsmsg_getstatus(struct mfsmsg_getstatus *sp,int f,char *mbuf,int mlen)
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
	        sp->msgtype = mfsmsgtype_getstatus ;
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
/* end subroutine (mfsmsg_getstatus) */


int mfsmsg_status(struct mfsmsg_status *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_rui(&m,&sp->queries) ;
	        serialbuf_ruc(&m,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_status ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wui(&m,sp->pid) ;
	        serialbuf_wui(&m,sp->queries) ;
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
/* end subroutine (mfsmsg_status) */


int mfsmsg_getval(struct mfsmsg_getval *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->w) ;
	        serialbuf_rstrw(&m,sp->key,MFSMSG_KEYLEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_getval ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->w) ;
	        serialbuf_wstrw(&m,sp->key,MFSMSG_KEYLEN) ;
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
/* end subroutine (mfsmsg_getval) */


int mfsmsg_val(struct mfsmsg_val *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->w) ;
	        serialbuf_ruc(&m,&sp->rc) ;
	        serialbuf_ruc(&m,&sp->vl) ;
	        serialbuf_rstrw(&m,sp->val,REALNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_val ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->w) ;
	        serialbuf_wuc(&m,sp->rc) ;
	        serialbuf_wuc(&m,sp->vl) ;
	        serialbuf_wstrw(&m,sp->val,REALNAMELEN) ;
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
/* end subroutine (mfsmsg_val) */


int mfsmsg_gethelp(struct mfsmsg_gethelp *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->idx) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_gethelp ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->idx) ;
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
/* end subroutine (mfsmsg_gethelp) */


int mfsmsg_help(struct mfsmsg_help *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->idx) ;
	        serialbuf_ruc(&m,&sp->rc) ;
	        serialbuf_ruc(&m,&sp->vl) ;
	        serialbuf_rstrw(&m,sp->val,REALNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_help ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->idx) ;
	        serialbuf_wuc(&m,sp->rc) ;
	        serialbuf_wuc(&m,sp->vl) ;
	        serialbuf_wstrw(&m,sp->val,REALNAMELEN) ;
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
/* end subroutine (mfsmsg_help) */


int mfsmsg_getname(struct mfsmsg_getname *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_rstrw(&m,sp->un,USERNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_getname ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wstrw(&m,sp->un,USERNAMELEN) ;
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
/* end subroutine (mfsmsg_getname) */


int mfsmsg_name(struct mfsmsg_name *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->rc) ;
	        serialbuf_rstrw(&m,sp->rn,REALNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_name ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->rc) ;
	        serialbuf_wstrw(&m,sp->rn,REALNAMELEN) ;
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
/* end subroutine (mfsmsg_name) */


int mfsmsg_getuser(struct mfsmsg_getuser *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_rstrw(&m,sp->spec,REALNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_getuser ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wstrw(&m,sp->spec,REALNAMELEN) ;
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
/* end subroutine (mfsmsg_getuser) */


int mfsmsg_user(struct mfsmsg_user *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->rc) ;
	        serialbuf_rstrw(&m,sp->un,USERNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_user ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->rc) ;
	        serialbuf_wstrw(&m,sp->un,USERNAMELEN) ;
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
/* end subroutine (mfsmsg_user) */


int mfsmsg_exit(struct mfsmsg_exit *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_rstrw(&m,sp->reason,REALNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_exit ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wstrw(&m,sp->reason,REALNAMELEN) ;
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
/* end subroutine (mfsmsg_exit) */


int mfsmsg_mark(struct mfsmsg_mark *sp,int f,char *mbuf,int mlen)
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
	        sp->msgtype = mfsmsgtype_mark ;
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
/* end subroutine (mfsmsg_mark) */


int mfsmsg_ack(struct mfsmsg_ack *sp,int f,char *mbuf,int mlen)
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
	        serialbuf_ruc(&m,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = mfsmsgtype_mark ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
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
/* end subroutine (mfsmsg_ack) */


int mfsmsg_getlistener(struct mfsmsg_getlistener *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&msgbuf,&sp->tag) ;

	        serialbuf_rui(&msgbuf,&sp->idx) ;

	    } else { /* write */

	        sp->msgtype = mfsmsgtype_getlistener ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wui(&msgbuf,sp->tag) ;

	        serialbuf_wui(&msgbuf,sp->idx) ;

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
/* end subroutine (mfsmsg_getlistener) */


int mfsmsg_listener(
struct mfsmsg_listener *sp,int f,char *mbuf,int mlen)
{
	SERIALBUF	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&msgbuf,&sp->tag) ;

	        serialbuf_rui(&msgbuf,&sp->idx) ;

	        serialbuf_rui(&msgbuf,&sp->pid) ;

	        serialbuf_ruc(&msgbuf,&sp->rc) ;

	        serialbuf_ruc(&msgbuf,&sp->ls) ;

	        serialbuf_rstrw(&msgbuf,sp->name,MFSMSG_LNAMELEN) ;

	        serialbuf_rstrw(&msgbuf,sp->addr,MFSMSG_LADDRLEN) ;

	    } else { /* write */

	        sp->msgtype = mfsmsgtype_listener ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wui(&msgbuf,sp->tag) ;

	        serialbuf_wui(&msgbuf,sp->idx) ;

	        serialbuf_wui(&msgbuf,sp->pid) ;

	        serialbuf_wuc(&msgbuf,sp->rc) ;

	        serialbuf_wuc(&msgbuf,sp->ls) ;

	        serialbuf_wstrw(&msgbuf,sp->name,MFSMSG_LNAMELEN) ;

	        serialbuf_wstrw(&msgbuf,sp->addr,MFSMSG_LADDRLEN) ;

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
/* end subroutine (mfsmsg_listener) */

