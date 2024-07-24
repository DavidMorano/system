/* pcs-msg SUPPORT */
/* lang=C++20 */

/* messages for PCS requests-responses */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the subroutines to make and parse the
	PCSMSG family of messages.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"pcsmsg.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsmsg_getstatus(pcsmsg_getstatus *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_getstatus ;
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
/* end subroutine (pcsmsg_getstatus) */

int pcsmsg_status(pcsmsg_status *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_status ;
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
/* end subroutine (pcsmsg_status) */

int pcsmsg_getval(pcsmsg_getval *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        serialbuf_rstrw(&m,sp->key,PCSMSG_KEYLEN) ;
	    } else { /* write */
	        sp->msgtype = pcsmsgtype_getval ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&m,hdr) ;
	        serialbuf_wui(&m,sp->tag) ;
	        serialbuf_wuc(&m,sp->w) ;
	        serialbuf_wstrw(&m,sp->key,PCSMSG_KEYLEN) ;
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
/* end subroutine (pcsmsg_getval) */

int pcsmsg_val(pcsmsg_val *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_val ;
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
/* end subroutine (pcsmsg_val) */

int pcsmsg_gethelp(pcsmsg_gethelp *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_gethelp ;
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
/* end subroutine (pcsmsg_gethelp) */

int pcsmsg_help(pcsmsg_help *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_help ;
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
/* end subroutine (pcsmsg_help) */

int pcsmsg_getname(pcsmsg_getname *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_getname ;
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
/* end subroutine (pcsmsg_getname) */

int pcsmsg_name(pcsmsg_name *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_name ;
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
/* end subroutine (pcsmsg_name) */

int pcsmsg_getuser(pcsmsg_getuser *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_getuser ;
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
/* end subroutine (pcsmsg_getuser) */

int pcsmsg_user(pcsmsg_user *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_user ;
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
/* end subroutine (pcsmsg_user) */

int pcsmsg_exit(pcsmsg_exit *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_exit ;
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
/* end subroutine (pcsmsg_exit) */

int pcsmsg_mark(pcsmsg_mark *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_mark ;
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
/* end subroutine (pcsmsg_mark) */

int pcsmsg_ack(pcsmsg_ack *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	m ;
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
	        sp->msgtype = pcsmsgtype_mark ;
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
/* end subroutine (pcsmsg_ack) */


