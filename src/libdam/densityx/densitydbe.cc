/* densitydbe SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* density DB entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-25, David A­D­ Morano
	This is being writen to support the DENSITYDB object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	densitydbe

	Description:
	These module implements the messages (reads and writes) to
	the DENSITYDB database file.  It actually does the subroutine
	marshalling for the file reads and writes.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"densitydbe.h"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int densitydbe_start(densitydbe *op) noex {
    	DENSITYDBE	*hop = op ;
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}

int densitydbe_finish(densitydbe *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}

int densitydbe_wr(densitydbe *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && mbuf) {
	    char	*buf = cast_const<charp>(mbuf) ;
	    if (mlen < 0) mlen = INT_MAX ;
	    if (serialbuf mb ; (rs = mb.start(buf,mlen)) >= 0) {
	        {
	            mb >> op->count ;
	            mb >> op->utime ;
	        }
	        rs1 = mb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (serialbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (densitydbe_wr) */

int densitydbe_rd(densitydbe *op,char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    if (mlen < 0) mlen = INT_MAX ;
	    if (serialbuf mb ; (rs = mb.start(mbuf,mlen)) >= 0) {
		{
		    mb << op->count ;
		    mb << op->utime ;
		}
	        rs1 = mb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (serialbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (densitydbe_rd) */

int densitydbe_entsz(densitydbe *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = (2 * szof(uint)) ;
	}
	return rs ;
}
/* end subroutine (densitydbe_entsz) */


/* private subroutines */

int densitydbe::wr(cchar *mbuf,int mlen) noex {
	return densitydbe_wr(this,mbuf,mlen) ;
}

int densitydbe::rd(char *mbuf,int mlen) noex {
	return densitydbe_rd(this,mbuf,mlen) ;
}

void densitydbe::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("densitydbe",rs,"fini-finish") ;
	}
}

densitydbe::operator int () noex {
	return densitydbe_entsz(this) ;
}

densitydbe_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case densitydbemem_start:
	        rs = densitydbe_start(op) ;
	        break ;
	    case densitydbemem_entsz:
	        rs = densitydbe_entsz(op) ;
	        break ;
	    case densitydbemem_finish:
	        rs = densitydbe_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (densitydbe_co::operator) */


