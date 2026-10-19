/* pathbuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* buffer object for path manipulation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	pathbuf

	Description:
	This object is used for path manipulation.

	Arguments:
	rbuf		buffer to receive cleaned up path string
	pbuf		supplied path to clean up
	plen		length of supplied path to clean up

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU |str{x}cpy(3usys)| */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"pathbuf.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import deb ;


/* local defines */

#define	PB_BUFP		pbuf
#define	PB_BUFL		plen
#define	PB_IDX		idx

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using usys::strwcpy ;			/* subroutine (unused) */
using usys::strbcpy ;			/* subroutine (unused) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local structures */


/* forward references */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pathbuf::start(char *rb,int rl) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent rl=%d\n",rl) ;
	if (rb) ylikely {
	    rs		= SR_OK ;
	    pbuf	= rb ;
	    plen	= rl ;
	    idx		= 0 ;
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pathbuf_start) */

int pathbuf::pfinish() noex {
	int		rs ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if ((rs = idx) >= 0) ylikely {
	    charp bp = (pbuf + idx) ;
	    len		= idx ;
	    *bp		= eos ;			/* NUL-terminate */
	    pbuf	= nullptr ;
	    plen	= 0 ;
	    idx		= SR_NOTOPEN ;
	} /* end if (ok) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end method (pathbuf::pfinish) */

int pathbuf::str(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (sp) ylikely {
	    if ((rs = idx) >= 0) {
	        charp ep = (pbuf + plen) ;
	        charp bp = (pbuf + idx) ;
	        while ((bp < ep) && sl && *sp) {
		    *bp++ = *sp++ ;
		    sl-- ;
	        } /* end for */
	        *bp = eos ;
	        if ((bp < ep) || isvalzero(sl) || isvalzero(*sp)) {
		    rs = intconv(bp - (pbuf + idx)) ;
	            idx += rs ;
	        } else {
		    idx = rs ;
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (pathbuf::str) */

int pathbuf::chr(int ch) noex {
    	int		rs ;
	if ((rs = idx) >= 0) {
	    rs = SR_OVERFLOW  ;
	    if (idx < plen) ylikely {
	        charp bp = (pbuf + idx) ;
	        *bp++ = char(ch) ;
		*bp = eos ;
		rs = 1 ; /* number of bytes stored */
		idx += rs ;
	    } else {
		idx = rs ;
	    } /* end if (good) */
	} /* end if (valid) */
	return rs ;
} /* end method (pathbuf::chr) */

int pathbuf::premslash() noex {
	int		rs ;
	DEBPRINTF("ent\n") ;
	if ((rs = PB_IDX) >= 0) ylikely {
	    charp bp = (PB_BUFP + PB_IDX) ;
	    if ((PB_BUFL > 0) && (bp[-1] == '/')) {
	        PB_BUFL -= 1 ;
	        bp -= 1 ;
	    } /* end if */
	    while ((PB_BUFL > 0) && (bp[-1] != '/')) {
	        PB_BUFL -= 1 ;
	        bp -= 1 ;
	    } /* end while */
	    if ((PB_BUFL > 0) && (bp[-1] == '/')) {
	        PB_BUFL -= 1 ;
	        bp -= 1 ;
	    } /* end if */
	    PB_IDX = intconv(bp - PB_BUFP) ;
	} /* end if (ok) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (pathbuf::premslash) */

pathbuf_co::operator int () noex {
    	using enum	pathbufmems ;
    	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->pbuf) ylikely {
	        switch (w) {
	        case pathbufmem_remslash:
	            rs = op->premslash() ;
		    break ;
	        case pathbufmem_finish:
	            rs = op->pfinish() ;
		    break ;
		default:
		    rs = SR_BUGCHECK ;
		    break ;
	        } /* end switch */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end method (pathbuf_co::operator) */


