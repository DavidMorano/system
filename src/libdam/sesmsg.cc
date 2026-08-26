/* sesmsg SUPPORT */
/* encoding=ISOCHAR_BITCHAR_BIT59-1 */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sesmsg

	Description:
	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| + |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdorder.h>		/* LIBUC */
#include	<serialbuf.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sesmsg.hh"		/* C++ only header */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int sesmsger_mbuf(SESMSG_MBUF *,int,int,char *,int) noex ;

local uchar mktype(int ch) noex {
    	return uchar(ch & UCHAR_MAX) ;
} /* end extern (C) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sesmsger_exit(SESMSG_EXIT *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rstrw(&mb,sp->reason,SESMSG_REASONLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_exit ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wstrw(&mb,sp->reason,SESMSG_REASONLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_exit) */

int sesmsger_noop(SESMSG_NOOP *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_noop ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_noop) */

int sesmsger_gen(SESMSG_GEN *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint		mt = sesmsgtype_gen ;
	return sesmsger_mbuf(mp,mt,f,mbuf,mlen) ;
} /* end subroutine (sesmsg_gen) */

int sesmsger_biff(SESMSG_BIFF *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint		mt = sesmsgtype_biff ;
	return sesmsger_mbuf(mp,mt,f,mbuf,mlen) ;
} /* end subroutine (sesmsg_biff) */

int sesmsger_echo(SESMSG_ECHO *sp,int f,char *mbuf,int mlen) noex {
	SESMSG_MBUF	*mp = (SESMSG_MBUF *) sp ;
	cint		mt = sesmsgtype_echo ;
	return sesmsger_mbuf(mp,mt,f,mbuf,mlen) ;
} /* end subroutine (sesmsg_echo) */

int sesmsger_response(SESMSG_RESPONSE *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_response) */

int sesmsger_passfd(SESMSG_PASSFD *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_passfd) */

int sesmsger_getsysmisc(SESMSG_GETSYSMISC *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getsysmisc ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_getsysmisc) */

int sesmsger_sysmisc(SESMSG_SYSMISC *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_sysmisc) */

int sesmsger_getloadave(SESMSG_GETLOADAVE *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getloadave ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_getloadve) */

int sesmsger_loadave(SESMSG_LOADAVE *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_loadave) */

int sesmsger_reploadave(SESMSG_REPLOADAVE *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_reploadave) */

int sesmsger_getlistener(SESMSG_GETLISTENER *sp,int f,
		char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_getlistener ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_getlistener) */

int sesmsger_listener(SESMSG_LISTENER *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_listener) */

int sesmsger_mark(SESMSG_MARK *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_mark ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_mark) */

int sesmsger_unknown(SESMSG_UNKNOWN *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_unknown ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_unknown) */

int sesmsger_gethelp(SESMSG_GETHELP *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rui(&mb,&sp->idx) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_gethelp ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wui(&mb,sp->idx) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_gethelp) */

int sesmsger_help(SESMSG_HELP *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
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
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
} /* end subroutine (sesmsg_help) */

int sesmsger_cmd(SESMSG_CMD *sp,int f,char *mbuf,int mlen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_rstrw(&mb,sp->cmd,SESMSG_CMDLEN) ;
	    } else { /* write */
	        sp->msgtype = sesmsgtype_cmd ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wstrw(&mb,sp->cmd,SESMSG_CMDLEN) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_cmd) */


/* local subroutines */

local int sesmsger_mbuf(SESMSG_MBUF *sp,int mt,int f,char *mbuf,int mlen) noex {
	cint		nlen = SESMSG_NBUFLEN ;
	cint		ulen = SESMSG_USERLEN ;
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) ylikely {
	    ulong	lw ;
	    uint	hdr ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        sp->msgtype = mktype(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        serialbuf_rul(&mb,&lw) ;
		sp->stime = (time_t) lw ;
	        serialbuf_rui(&mb,&sp->tag) ;
	        serialbuf_ruc(&mb,&sp->rc) ;
	        serialbuf_rstrw(&mb,sp->user,ulen) ;
	        serialbuf_rstrw(&mb,sp->nbuf,nlen) ;
	    } else { /* write */
	        sp->msgtype = mktype(mt) ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&mb,hdr) ;
		lw = (ulong) sp->stime ;
	        serialbuf_wul(&mb,lw) ;
	        serialbuf_wui(&mb,sp->tag) ;
	        serialbuf_wuc(&mb,sp->rc) ;
	        serialbuf_wstrw(&mb,sp->user,ulen) ;
	        serialbuf_wstrw(&mb,sp->nbuf,nlen) ;
	        if ((sp->msglen = serialbuf_getlen(&mb)) > 0) {
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(mbuf,hdr) ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
} /* end subroutine (sesmsg_mbuf) */


