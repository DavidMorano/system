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
#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<inttypes.h>
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

int msgide_all::start() noex {
    	int		rs ;
	static cint		rsv = var ;
	if ((rs = rsv) >= 0) {
	    cint	maxhost = var.maxhostlen ;
	    cint	sz = ((var.reciplen + 1) + (2 * (var.maxhostlen + 1))) ;
	    int		ai = 0 ;
	    if ((rs = uc_malloc(sz,&a)) >= 0) {
		recipient = (a + (maxhost * ai++)) ;
		messageid = (a + (maxhost * ai++)) ;
		from = (a + (maxhost * ai++)) ;
		len.from = maxhost ;
		len.messageid = maxhost ;
		len.recipient = var.reciplen ;
	    }
	} /* end if (vars) */
	return rs ;
}

int msgide_all::finish() noex {
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

int msgide_update::start() noex {
    	return SR_OK ;
}

int msgide_update::finish() noex {
    	return SR_OK ;
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
/* end subroutine (entry_update::rd) */



/* local subroutines */

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


