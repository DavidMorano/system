/* mcmsg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecstr.h>
#include	<stdorder.h>
#include	<serialbuf.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mcmsg.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mcmsg_req::rd(char *abuf,int alen) noex {
    	cint	frd = true ;
    	return mcmsg_request(this,frd,abuf,alen) ;
}

int mcmsg_req::wr(cchar *abuf,int alen) noex {
    	char	*wbuf = cast_const<charp>(abuf) ;
    	cint	frd = false ;
    	return mcmsg_request(this,frd,wbuf,alen) ;
}

int mcmsg_request(mcmsg_req *sp,int f,char *abuf,int alen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(abuf,alen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        sp->msgtype = mcmsgtype_request ;
	        hdr = sp->msgtype ;
	        mb << hdr ;
	        mb << sp->seq ;
	        mb << sp->tag ;
	        mb << sp->timestamp ;
	        mb.wstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.wstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	        if (((sp->msglen = mb.getlen)) > 0) { /* <- intended */
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(abuf,hdr) ;
	        }
	    } else { /* write */
	        mb.rui(&hdr) ;
	        sp->msgtype = uchar(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        mb >> sp->seq ;
	        mb >> sp->tag ;
	        mb >> sp->timestamp ;
	        mb.rstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.rstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_request) */

int mcmsg_rep::rd(char *abuf,int alen) noex {
    	cint	frd = true ;
    	return mcmsg_report(this,frd,abuf,alen) ;
}

int mcmsg_rep::wr(cchar *abuf,int alen) noex {
    	char	*wbuf = cast_const<charp>(abuf) ;
    	cint	frd = false ;
    	return mcmsg_report(this,frd,wbuf,alen) ;
}

int mcmsg_report(mcmsg_rep *sp,int f,char *abuf,int alen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(abuf,alen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        sp->msgtype = mcmsgtype_report ;
	        hdr = sp->msgtype ;
	        mb << hdr ;
	        mb << sp->seq ;
	        mb << sp->tag ;
	        mb << sp->timestamp ;
	        mb << sp->mlen ;
	        mb.wstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.wstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	        mb.wstrw(sp->msgid,MCMSG_LMSGID) ;
	        mb.wstrw(sp->from,MCMSG_LFROM) ;
	        mb << sp->flags ;
	        mb << sp->rc ;
	        if (((sp->msglen = mb.getlen)) > 0) { /* <- intended */
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(abuf,hdr) ;
	        }
	    } else { /* write */
	        mb >> hdr ;
	        sp->msgtype = uchar(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        mb >> sp->seq ;
	        mb >> sp->tag ;
	        mb >> sp->timestamp ;
	        mb >> sp->mlen ;
	        mb.rstrw(sp->cluster,MCMSG_LCLUSTER) ;
	        mb.rstrw(sp->mailuser,MCMSG_LMAILUSER) ;
	        mb.rstrw(sp->msgid,MCMSG_LMSGID) ;
	        mb.rstrw(sp->from,MCMSG_LFROM) ;
	        mb >> sp->flags ;
	        mb >> sp->rc ;
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_report) */

int mcmsg_ack::rd(char *abuf,int alen) noex {
    	cint	frd = true ;
    	return mcmsg_acknowledge(this,frd,abuf,alen) ;
}

int mcmsg_ack::wr(cchar *abuf,int alen) noex {
    	char	*wbuf = cast_const<charp>(abuf) ;
    	cint	frd = false ;
    	return mcmsg_acknowledge(this,frd,wbuf,alen) ;
}

int mcmsg_acknowledge(mcmsg_ack *sp,int f,char *abuf,int alen) noex {
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(abuf,alen)) >= 0) {
	    uint	hdr ;
	    if (f) { /* read */
	        sp->msgtype = mcmsgtype_ack ;
	        hdr = sp->msgtype ;
		mb << hdr ;
	        mb << sp->seq ;
	        mb << sp->tag ;
	        mb << sp->rc ;
	        if (((sp->msglen = mb.getlen)) > 0) { /* <- intended */
	            hdr |= (sp->msglen << CHAR_BIT) ;
	            stdorder_wui(abuf,hdr) ;
	        }
	    } else { /* write */
	        mb >> hdr ;
	        sp->msgtype = uchar(hdr) ;
	        sp->msglen = (hdr >> CHAR_BIT) ;
	        mb >> sp->seq ;
	        mb >> sp->tag ;
	        mb >> sp->rc ;
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (mcmsg_acknowledge) */


/* local subroutines */

local int mcmsg_reqstart(mcmsg_req *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

local int mcmsg_reqfinish(mcmsg_req *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

local int mcmsg_repstart(mcmsg_rep *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

local int mcmsg_repfinish(mcmsg_rep *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

local int mcmsg_ackstart(mcmsg_ack *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

local int mcmsg_ackfinish(mcmsg_ack *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
    	return rs ;
}

void mcmsg_req::dtor() noex {
    	if (cint rs = finish ; rs < 0) {
	    ulogerror("mcmsg_req",rs,"dtor-finish") ;
	}
}

void mcmsg_rep::dtor() noex {
    	if (cint rs = finish ; rs < 0) {
	    ulogerror("mcmsg_rep",rs,"dtor-finish") ;
	}
}

void mcmsg_ack::dtor() noex {
    	if (cint rs = finish ; rs < 0) {
	    ulogerror("mcmsg_ack",rs,"dtor-finish") ;
	}
}

int mcmsg_coreq(mcmsg_req *op,int w) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case mcmsgmem_start:
		rs = mcmsg_reqstart(op) ;
		break ;
	    case mcmsgmem_finish:
		rs = mcmsg_reqfinish(op) ;
		break ;
	    }
	}
	return rs ;
}

int mcmsg_corep(mcmsg_rep *op,int w) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case mcmsgmem_start:
		rs = mcmsg_repstart(op) ;
		break ;
	    case mcmsgmem_finish:
		rs = mcmsg_repfinish(op) ;
		break ;
	    }
	}
	return rs ;
}

int mcmsg_coack(mcmsg_ack *op,int w) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case mcmsgmem_start:
		rs = mcmsg_ackstart(op) ;
		break ;
	    case mcmsgmem_finish:
		rs = mcmsg_ackfinish(op) ;
		break ;
	    }
	}
	return rs ;
}


