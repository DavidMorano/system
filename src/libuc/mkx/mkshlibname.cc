/* mkshlibname SUPPORT */
/* lang=C++20 */

/* make the filename for a shared library (shared object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine formulates (makes) the file-name for a
	shared library (shared object).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

static bufsizevar	maxnamelen(getbufsize_mn) ;


/* exported variables */


/* exported subroutines */

int mkshlibname(char *shlibname,cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (shlibname && pnp) {
	    rs = SR_INVALID ;
	    if (pnp[0]) {
	        cchar	*lc = "lib" ;
		if (pnl < 0) pnl = strlen(pnp) ;
		if ((rs = maxnamelen) >= 0) {
		    cint	shliblen = rs ;
	            bool	f = ((pnl >= 3) && (strncmp(pnp,lc,3) == 0)) ;
	            if (! f) {
	                rs = storebuf_strw(shlibname,shliblen,i,lc,3) ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(shlibname,shliblen,i,pnp,pnl) ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_char(shlibname,shliblen,i,'.') ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(shlibname,shliblen,i,"so",2) ;
	                i += rs ;
	            }
		} /* end if (maxnamelen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkshlibname) */


