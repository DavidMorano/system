/* msgide SUPPORT */
/* encoding=ISO8859-1 */
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

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<serialbuf.h>
#include	<localmisc.h>

import msgide ;

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

int msgide_all::istart() noex {
    	int		rs ;
	int		esz = 0 ;
	static cint		rsv = var ;
	if ((rs = rsv) >= 0) {
	    cint	maxhost = var.maxhostlen ;
	    cint	sz = ((var.reciplen + 1) + (2 * (var.maxhostlen + 1))) ;
	    int		ai = 0 ;
	    if ((rs = uc_malloc(sz,&a)) >= 0) {
		recipient = (a + ((maxhost + 1) * ai++)) ;
		messageid = (a + ((maxhost + 1) * ai++)) ;
		from = (a + (maxhost * ai++)) ;
		len.from = maxhost ;
		len.messageid = maxhost ;
		len.recipient = var.reciplen ;
		esz += sz ;
		esz += szof(count) ;
		esz += szof(utime) ;
		esz += szof(ctime) ;
		esz += szof(mtime) ;
		esz += szof(hash) ;
		len.entsz = esz ;
	    } /* end if (memory-allocation) */
	} /* end if (vars) */
	return (rs >= 0) ? esz : rs ;
}

int msgide_all::ifinish() noex {
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

int msgide_all::wr(cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
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
/* end method (msgide_all::wr) */

int msgide_all::rd(char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
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
/* end subroutine (entry_all::rd) */

int msgide_update::istart() noex {
    	int		rs = SR_OK ;
	rs += szof(count) ;
	rs += szof(utime) ;
	len.entsz = rs ;
    	return rs ;
}

int msgide_update::ifinish() noex {
    	int		rs = SR_OK ;
    	return rs ;
}

int msgide_update::wr(cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
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
/* end method (msgide_update::wr) */

int msgide_update::rd(char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbuf) {
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
/* end subroutine (msgide_update::rd) */


/* local subroutines */

template<> 
msgide_co<msgide_all>::operator int () noex {
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
/* end method (msgide_co<msgide_all>::operator) */

template<> 
msgide_co<msgide_update>::operator int () noex {
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
/* end method (msgide_co<msgide_update>::operator) */

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


