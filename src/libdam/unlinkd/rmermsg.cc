/* rmermsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message for UNLINKD program */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	rmermsg

	Description:
	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

	Synopsis:
	int rmermsg_proc(rmermsg *op,int f,char *mbuf,int mlen) noex

	Arguments:
	op		object pointer
	g		cmd: 0=write, 1=read
	mbuf		buffer pointer
	mlen		buffer length

	Returns:
	>=0		ok
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<serialbuf.h>		/* LIBUC */
#include	<stdorder.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBUC */

#include	"rmermsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */



/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxpathlen ;
	int operator () (int) noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int rmermsg_start(rmermsg *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    if (char *bp ; (rs = mem.mp(&bp)) >= 0) {
		if (static cint rsv = var(rs) ; (rs = rsv) >= 0) {
		    op->fname = bp ;
		}
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (rmermsg_start) */

int rmermsg_finish(rmermsg *op) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = memclear(op) ;
	    if (op->fname) {
		rs1 = mem.free(op->fname) ;
		if (rs >= 0) rs = rs1 ;
		op->fname = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (rmermsg_finish) */

int rmermsg_proc(rmermsg *op,int f,char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	rs = SR_NOTOPEN ;
	if (op->fname) {
	if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    int		v ;
	    if (f) { /* read */
	        serialbuf_rui(&mb,&hdr) ;
	        op->msgtype	= conv<uchar>(hdr & UCHAR_MAX) ;
	        op->msglen	= conv<ushort>(hdr >> CHAR_BIT) ;
		mb >> op->tag ;
		mb >> op->delay ;
		mb >> op->sysid ;
		mb >> v ;
		op->uid = v ;
	        mb.rstrw(op->fname,var.maxpathlen) ;
	    } else { /* write */
	        op->msgtype = rmermsgtype_fname ;
	        hdr = op->msgtype ;
	        mb << hdr ;
		mb << op->tag ;
		mb << op->delay ;
		mb << op->sysid ;
	        v = op->uid ;
	        mb << v ;
	        mb.wstrw(op->fname,-1) ;
	        if ((v = mb.getlen) > 0) {
		    op->msglen = conv<ushort>(v) ;
	            hdr |= (op->msglen << CHAR_BIT) ;
	            mb << hdr ;
	        }
	    } /* end if */
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (rmermsg_proc) */

int rmermsg_wr(rmermsg *op,cchar *wbuf,int wlen) noex {
    	char *mbuf = cast_const<charp>(wbuf) ;
    	return rmermsg_proc(op,0,mbuf,wlen) ;
} /* end subroutine (rmermsg_wr) */

int rmermsg_rd(rmermsg *op,char *rbuf,int rlen) noex {
    	return rmermsg_proc(op,1,rbuf,rlen) ;
} /* end subroutine (rmermsg_wr) */


/* private subroutines */

int rmermsg::wr(cchar *wbuf,int wlen) noex {
	return rmermsg_wr(this,wbuf,wlen) ;
} /* end method (rmermsg::wr) */

int rmermsg::rd(char *rbuf,int rlen) noex {
	return rmermsg_rd(this,rbuf,rlen) ;
} /* end method (rmermsg::wr) */

void rmermsg::dtor() noex {
    if (cint rs = finish ; rs < 0) {
	ulogerror("rmermsg",rs,"fini-finish") ;
    } /* end if */
} /* end method (rmermsg::dtor) */

rmermsg_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case rmermsgmem_start:
	        rs = rmermsg_start(op) ;
	        break ;
	    case rmermsgmem_finish:
	        rs = rmermsg_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (rmermsg_co::operator) */

int vars::operator () (int v) noex {
    	int		rs = SR_BUGCHECK ;
	if (v >= 0) {
    	    maxpathlen = v ;
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (vars::operator) */


