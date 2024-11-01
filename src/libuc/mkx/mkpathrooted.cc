/* mkpathrooted SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a file rooted from the present-working-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkpathrooted

	Description:
	This subroutine examines the source file or path given
	and returns either the original source-path as it is
	or an absolute-path version it, depending on whether the
	source as already absolute or not.

	Synopsis:
	int mkpathrooted(char *rbuf,cchar *sbuf) noex

	Arguments:
	rbuf		result buffer
	sbuf		source buffer

	Returns:
	>=0		length of resulting c-string
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mkpath.h>
#include	<libmallocxx.h>
#include	<getpwd.h>
#include	<localmisc.h>

#include	"mkx.h"
#include	"mkpathrooted.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkpathrooted(char *rbuf,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        if (fn[0] != '/') {
	            if (char *pbuf{} ; (rs = libmalloc_mp(&pbuf)) >= 0) {
	                cint	plen = rs ;
	                if ((rs = getpwd(pbuf,plen)) >= 0) {
	                    rs = mkpath(rbuf,pbuf,fn) ;
		            rl = rs ;
	                }
	                rs1 = uc_libfree(pbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (rooting file) */
	        } else {
		    rs = mkpath(rbuf,fn) ;
		    rl = rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end if (mkpathrooted) */


