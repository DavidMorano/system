/* msgide SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message identification (MSG-ID) entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-25, David A­D­ Morano
	This is being writen to support the MSGID object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	msgide

	Description:
	These module implements the messages (reads and writes) to
	the MSGID database file.  It actually does the subroutine
	marshalling for the file reads and writes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<serialbuf.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"msgide.hh"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		reciplen ;
	int		maxhostlen ;
	operator int () noex ;
    } ;
}

/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int msgide::istart() noex {
    	int		rs ;
	int		esz = 0 ;
	static cint	rsv = var ;
	if ((rs = rsv) >= 0) {
	    cint	maxhost = var.maxhostlen ;
	    cint	sz = ((var.reciplen + 1) + (2 * (var.maxhostlen + 1))) ;
	    int		ai = 0 ;
	    if ((rs = uc_malloc(sz,&a)) >= 0) {
		from 		= (a + ((maxhost + 1) * ai++)) ;
		messageid 	= (a + ((maxhost + 1) * ai++)) ;
		recipient 	= (a + ((maxhost + 1) * ai++)) ;
		len.from = maxhost ;
		len.messageid = maxhost ;
		len.recipient = var.reciplen ;
		esz += sz ;
		esz += szof(count) ;
		esz += szof(utime) ;
		esz += szof(ctime) ;
		esz += szof(mtime) ;
		esz += szof(hash) ;
		len.entsz = iceil(esz,szof(int)) ;
		esz = len.entsz ;
	    } /* end if (memory-allocation) */
	} /* end if (vars) */
	return (rs >= 0) ? esz : rs ;
}

int msgide::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (a) {
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    {
		recipient = nullptr ;
		messageid = nullptr ;
		from = nullptr ;
	    }
	}
	return rs ;
}

int msgide::wr(cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
	    if (mlen < 0) mlen = len.entsz ;
	    rs = SR_INVALID ;
	    if (mlen > 0) {
		cint	maxhost = var.maxhostlen ;
		char	*buf = cast_const<charp>(mbuf) ;
	        if (serialbuf sb ; (rs = sb.start(buf,mlen)) >= 0) {
		    {
			sb >> count ;
			sb >> utime ;
			sb >> ctime ;
			sb >> mtime ;
			sb >> hash ;
			sb.rstrn(recipient,len.recipient) ;
			sb.rstrn(messageid,maxhost) ;
			sb.rstrn(from,maxhost) ;
	    	    }
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} 
/* end method (msgide::wr) */

int msgide::wru(cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
	    if (mlen < 0) mlen = len.entsz ;
	    rs = SR_INVALID ;
	    if (mlen > 0) {
		char	*buf = cast_const<charp>(mbuf) ;
	        if (serialbuf sb ; (rs = sb.start(buf,mlen)) >= 0) {
		    {
			sb >> count ;
			sb >> utime ;
	    	    }
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} 
/* end method (msgide::wru) */

int msgide::rd(char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
	    if (mlen < 0) mlen = len.entsz ;
	    rs = SR_INVALID ;
	    if (mlen > 0) {
		cint	maxhost = var.maxhostlen ;
	        if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
		    {
			sb << count ;
			sb << utime ;
			sb << mtime ;
			sb << hash ;
	    		sb.wstrn(recipient,len.recipient) ;
	    		sb.wstrn(messageid,maxhost) ;
	    		sb.wstrn(from,maxhost) ;
		    }
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgide::rd) */

int msgide::rdu(char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
	    if (mlen < 0) mlen = len.entsz ;
	    rs = SR_INVALID ;
	    if (mlen > 0) {
	        if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
		    {
			sb << count ;
			sb << utime ;
		    }
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (msgide::rdu) */

int msgide::loadrec(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if (sl < 0) sl = lenstr(sp,len.recipient) ;
	    rs = SR_INVALID ;
	    if (sl > 0) {
		rs = snwcpy(recipient,len.recipient,sp,sl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (msgide::loadrec) */

int msgide::loadmid(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if (sl < 0) sl = lenstr(sp,len.messageid) ;
	    rs = SR_INVALID ;
	    if (sl > 0) {
		rs = snwcpy(messageid,len.messageid,sp,sl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (msgide::loadmid) */


/* local subroutines */

template<> 
msgide_co<msgide>::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case msgidemem_start:
	        rs = op->istart() ;
	        break ;
	    case msgidemem_entsz:
	        rs = op->len.entsz ;
	        break ;
	    case msgidemem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (msgide_co<msgide>::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    reciplen = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		maxhostlen = rs ;
	    }
	}
	return rs ;
}


