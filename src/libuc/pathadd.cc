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
	pathadd
	pathaddx

	Description:
	This subroutine adds a new component to an existing file-path.
	These subroutines return the total length of the final path.

	Synopsis:
	int pathaddw(char *pbuf,int plen,cchar *sp,int sl) noex
	int pathadd(char *pbuf,int plen,cchar *sp) noex

	Arguments:
	pbuf		existing path
	plen		length of existing path
	sp		new-componment pointer
	sl		new-componment length

	Returns:
	>=0		new length of new path; total length from beginning
	<0		error (system-return)

	Note:
	These subroutines return the total length of the final path 
	that resides in the given path-result-buffer.
	If you want a subroutine that returns just the added part,
	then check out the |storebuf(3uc)| facility or the |snadd(3uc)|
	facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"pathadd.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int pathaddw(char *pbuf,int pl,cchar *np,int nl) noex {
	int		rs = SR_FAULT ;
	if (pbuf && np) {
	    if ((rs = maxpathlen) >= 0) {
		cint	plen = rs ;
	        if ((rs >= 0) && (pl > 0) && (pbuf[pl-1] != '/')) {
	            rs = storebuf_char(pbuf,plen,pl,'/') ;
	            pl += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(pbuf,plen,pl,np,nl) ;
	            pl += rs ;
	        }
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (pathaddw) */

int pathadd(char *pbuf,int pl,cchar *np) noex {
	return pathaddw(pbuf,pl,np,-1) ;
}
/* end subroutine (pathadd) */


