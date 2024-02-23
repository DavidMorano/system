/* getprojname SUPPORT */
/* lang=C++20 */

/* get the default project for a given username (UNIX® Solaris® thing) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine is an extraction of code from elsewhere.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprojname

	Description:
	This subroutine retrieves the default project for a given
	username.

	Synopsis:
	int getprojname(char *rbuf,int rlen,cchar *un) noex

	Arguments:
	rbuf		buffer to receive the nodename 
	rlen		length of supplied buffer (should be PROJNAMELEN)
	un		username to lookup

	Returns:
	>=0		length of retrieved nodename
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getxusername.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"getprojname.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getprojname(char *nbuf,int nlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nl = 0 ;
	if (nbuf && un) {
	    if ((rs = getbufsize(getbufsize_un)) >= 0) {
	        cint	ulen = rs ;
		char	ubuf[rs+1] ;
	        if ((un[0] == '-') || (un[0] == '\0')) {
	            un = ubuf ;
	            rs = getusername(ubuf,ulen,-1) ;
	        }
	        if (rs >= 0) {
	            char	*pjbuf{} ;
	            if ((rs = malloc_pj(&pjbuf)) >= 0) {
			auto	getpj = uc_getdefaultproj ;
	                ucentpj	pj ;
	                cint	pjlen = rs ;
	                if ((rs = getpj(&pj,pjbuf,pjlen,un)) >= 0) {
	                    rs = sncpy1(nbuf,nlen,pj.pj_name) ;
		            nl = rs ;
		        }
		        rs1 = uc_free(pjbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (memory-allocation) */
	        } /* end if (ok) */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (getprojname) */


