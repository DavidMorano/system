/* pathadd SUPPORT */
/* lang=C++20 */

/* add a component to an existing path */
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
	int pathaddw(char *pbuf,int plen,cchar *sp,int sl) noex
	int pathadd(char *pbuf,int plen,cchar *sp) noex

	Arguments:
	pbuf		result buffer pointer
	plen		result buffer length
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
	subroutines (and other methods also).  In this subroutines,
	the length of the supplied buffer for the added-to path is
	assumed to be MAXPATHLEN (determined dynamically).


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>

#include	"pathadd.h"


/* local defines */


/* local namespaces */


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
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (pathaddw) */


