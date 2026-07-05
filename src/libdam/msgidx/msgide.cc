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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<serialbuf.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"msgide.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int msgide::istart() noex {
    	int		rs ;
	int		esz = 0 ; /* return-value */
	if (static cint rsv = var ; (rs = rsv) >= 0) {
	    cint	maxhost = var.maxhostlen ;
	    cint	sz = ((var.reciplen + 1) + (2 * (var.maxhostlen + 1))) ;
	    int		ai = 0 ;
	    if ((rs = lm_mall(sz,&a)) >= 0) {
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
} /* end method */

int msgide::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (a) {
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    {
		recipient = nullptr ;
		messageid = nullptr ;
		from = nullptr ;
	    }
	}
	return rs ;
} /* end method */

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
} /* end method (msgide::wr) */

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
} /* end method (msgide::wru) */

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
} /* end subroutine (msgide::rd) */

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
} /* end method (msgide::rdu) */

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
} /* end method (msgide::loadrec) */

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
} /* end method (msgide::loadmid) */


/* private subroutines */

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
} /* end method (msgide_co<msgide>::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
	    reciplen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
		maxhostlen = rs ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */


