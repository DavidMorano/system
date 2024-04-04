/* pathadd SUPPORT */
/* lang=C++20 */

/* add a component (c-string) to an existing path (c-string) */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathaddw
	pathadd

	Description:
	This subroutine adds a new component to an existing file-path.
	These subroutines return the total length of the final path.
	The length of the supplied buffer for the added-to path is
	assumed to be MAXPATHLEN (determined dynamically).

	Synopses:
	int pathaddw(char *pbuf,int pl,cchar *sp,int sl) noex
	int pathadd(char *pbuf,int pl,cchar *sp) noex

	Arguments:
	pbuf		result buffer pointer
	pl		length of result buffer that is already filled
	sp		new-componment c-string pointer
	sl		new-componment c-string length

	Returns:
	>=0		new length of new path; total length from beginning
	<0		error (system-return)

	Note:
	These subroutines return the total length of the final path
	that resides in the given path-result-buffer.  If you want
	a subroutine that returns just the added part, then check
	out the |storebuf(3uc)| facility or the |snadd(3uc)| facility,
	or simply create your own using the various |snx(3uc)| 
	subroutines (and other methods also).  In these subroutines,
	the length of the supplied buffer for the added-to path is
	assumed to be MAXPATHLEN (determined dynamically).

	Extra:
	The |pathaddw| subroutine is almost equivalent to:
	    int pathaddw(char *rbuf,int rl,cc *sp,int sl) noex {
	        int	rs ;
	        if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		    cint	rlen = rs ;
	            rs = storebuf_strw(rbuf,rlen,rl,sp,sl) ;
		    rl += rs ;
	        }
	        return (rs >= 0) ? rl : rs ;
	    }

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>

#include	"pathadd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int pathaddw(char *pbuf,int pl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pbuf && sp) {
	    rs = SR_INVALID ;
	    if (pl >= 0) {
	        if ((rs = maxpathlen) >= 0) {
		    cint	plen = rs ;
	            if ((pl > 0) && (pbuf[pl-1] != '/')) {
	                rs = storebuf_char(pbuf,plen,pl,'/') ;
	                pl += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(pbuf,plen,pl,sp,sl) ;
	                pl += rs ;
	            }
	        } /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (pathaddw) */


