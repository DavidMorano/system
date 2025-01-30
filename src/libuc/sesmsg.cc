/* sesmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<usystem.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"sesmsg.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */

static int sesmsg_mbuf(SESMSG_MBUF *,int,int,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sesmsg_exit(SESMSG_EXIT *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rstrw(&mb,sp->reason,SESMSG_REASONLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_exit ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wstrw(&mb,sp->reason,SESMSG_REASONLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_exit) */

int sesmsg_noop(SESMSG_NOOP *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_noop ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_noop) */

int sesmsg_gen(SESMSG_GEN *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint	mt = sesmsgtype_gen ;
	return sesmsg_mbuf(mp,mt,f,mbuf,mlen) ;
}
/* end subroutine (sesmsg_gen) */

int sesmsg_biff(SESMSG_BIFF *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint	mt = sesmsgtype_biff ;
	return sesmsg_mbuf(mp,mt,f,mbuf,mlen) ;
}
/* end subroutine (sesmsg_biff) */

int sesmsg_echo(SESMSG_ECHO *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint	mt = sesmsgtype_echo ;
	return sesmsg_mbuf(mp,mt,f,mbuf,mlen) ;
}
/* end subroutine (sesmsg_echo) */

int sesmsg_response(SESMSG_RESPONSE *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->pid) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_response ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->pid) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_response) */

int sesmsg_passfd(SESMSG_PASSWD *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rstrw(&mb,sp->svc,SESMSG_SVCLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_passfd ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wc(&mb,sp->msgtype) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wstrw(&mb,sp->svc,SESMSG_SVCLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_passfd) */


int sesmsg_getsysmisc(SESMSG_GETSYSMISC *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getsysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_getsysmisc) */

int sesmsg_sysmisc(SESMG_SYSMISC *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->la_1min) ;
	        serialbuf_rui(&mb,&sp->la_5min) ;
	        serialbuf_rui(&mb,&sp->la_15min) ;
	        serialbuf_rui(&mb,&sp->boottime) ;
	        serialbuf_rui(&mb,&sp->nproc) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_sysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->la_1min) ;
	        serialbuf_wui(&mb,sp->la_5min) ;
	        serialbuf_wui(&mb,sp->la_15min) ;
	        serialbuf_wui(&mb,sp->boottime) ;
	        serialbuf_wui(&mb,sp->nproc) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_sysmisc) */

int sesmsg_getloadave(SESMG_GETLOADAVE *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getloadave ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_getloadve) */

int sesmsg_loadave(SESMSG_LOADVE *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->la_1min) ;
	        serialbuf_rui(&mb,&sp->la_5min) ;
	        serialbuf_rui(&mb,&sp->la_15min) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_loadave ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->la_1min) ;
	        serialbuf_wui(&mb,sp->la_5min) ;
	        serialbuf_wui(&mb,sp->la_15min) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_loadave) */

int sesmsg_reploadave(sesmsg_reploadave *sp,f,mbuf,mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->utag) ;
	        serialbuf_rui(&mb,&sp->duration) ;
	        serialbuf_rui(&mb,&sp->interval) ;
	        serialbuf_rus(&mb,&sp->addrfamily) ;
	        serialbuf_rus(&mb,&sp->addrport) ;
	        serialbuf_rui(&mb,&sp->addrhost[0]) ;
	        serialbuf_rui(&mb,&sp->addrhost[1]) ;
	        serialbuf_rui(&mb,&sp->addrhost[2]) ;
	        serialbuf_rui(&mb,&sp->addrhost[3]) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_reploadave ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->utag) ;
	        serialbuf_wui(&mb,sp->duration) ;
	        serialbuf_wui(&mb,sp->interval) ;
	        serialbuf_wus(&mb,sp->addrfamily) ;
	        serialbuf_wus(&mb,sp->addrport) ;
	        serialbuf_wui(&mb,sp->addrhost[0]) ;
	        serialbuf_wui(&mb,sp->addrhost[1]) ;
	        serialbuf_wui(&mb,sp->addrhost[2]) ;
	        serialbuf_wui(&mb,sp->addrhost[4]) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_reploadave) */

int sesmsg_getlistener(sesmsg_getlistener *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getlistener ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_getlistener) */

int sesmsg_listener(sesmsg_listener *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	        serialbuf_rui(&mb,&sp->pid) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	        serialbuf_ruc(&mb,&sp->ls) ;
	        serialbuf_rstrw(&mb,sp->name,SESMSG_LNAMELEN) ;
	        serialbuf_rstrw(&mb,sp->addr,SESMSG_LADDRLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_listener ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        serialbuf_wui(&mb,sp->pid) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        serialbuf_wuc(&mb,sp->ls) ;
	        serialbuf_wstrw(&mb,sp->name,SESMSG_LNAMELEN) ;
	        serialbuf_wstrw(&mb,sp->addr,SESMSG_LADDRLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_listener) */

int sesmsg_mark(sesmsg_mark *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_mark ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_mark) */

int sesmsg_unknown(sesmsg_unknown *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_unknown ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_unknown) */

int sesmsg_gethelp(sesmsg_gethelp *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_gethelp ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_gethelp) */

int sesmsg_help(sessmsg_help *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	        serialbuf_rui(&mb,&sp->pid) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	        serialbuf_rstrw(&mb,sp->name,SESMSG_LNAMELEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_help ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        serialbuf_wui(&mb,sp->pid) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        serialbuf_wstrw(&mb,sp->name,SESMSG_LNAMELEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (sesmsg_help) */

int sesmsg_cmd(sesmsg_cmd *sp,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rstrw(&mb,sp->cmd,SESMSG_CMDLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_cmd ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wstrw(&mb,sp->cmd,SESMSG_CMDLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_cmd) */


/* local subroutines */

static int sesmsg_mbuf(sesmsg_mbuf *sp,int mt,int f,char *mbuf,int mlen) noex {
	serialbuf	mb ;
	cint		nlen = SESMSG_NBUFLEN ;
	cint		ulen = SESMSG_USERLEN ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&mb,mbuf,mlen)) >= 0) {
	    ulong	lw ;
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;
	        serialbuf_rul(&mb,&lw) ;
		sp->stime = (time_t) lw ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	        serialbuf_rstrw(&mb,sp->user,ulen) ;
	        serialbuf_rstrw(&mb,sp->nbuf,nlen) ;
	    } else { /* write */
	        sp->msgtype = (uchar) (mt&UCHAR_MAX) ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
		lw = (ulong) sp->stime ;
	        serialbuf_wul(&mb,lw) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        serialbuf_wstrw(&mb,sp->user,ulen) ;
	        serialbuf_wstrw(&mb,sp->nbuf,nlen) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = serialbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (sesmsg_mbuf) */


