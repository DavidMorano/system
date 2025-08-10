/* ucpathconf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* POSIX path-configuration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mkx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_pathconf(cchar *fname,int name,long *rp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    cint	elen = rs ;
	    if (char *ebuf ; (rs = uc_libmalloc((elen+1),&ebuf)) >= 0) {
	        if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		    rs = u_pathconf(ebuf,name,rp) ;
		    c = rs ;
	        } else if (rs == 0) {
		    rs = u_pathconf(fname,name,rp) ;
		    c = rs ;
	        }
	        rs1 = uc_libfree(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uc_pathconf) */


