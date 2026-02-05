/* bbhosts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the list of BB hosts from a PCS site BB hosts file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was adapted from a prior version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bbhosts

	Description:
	We just store node (host) names for use later in broadcasting
	the message around.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"bbhosts.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct loader {
	bbhosts		*op ;
	cchar		*pr ;
	cchar		*fn ;
	loader(bbhosts *o,cchar *p,cchar *f) noex : op(o), pr(p), fn(f) { } ;
	operator int () noex ;
    } ; /* end struct (laoder) */
} /* end namespace */

/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bbhosts_start(bbhosts *op,cchar *pr,cchar *bbhostfn) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if (op && pr && bbhostfn) {
	    rs = SR_INVALID ;
	    if (pr[0] && bbhostfn[0]) {
		loader lo(op,pr,bbhostfn) ;
		rs = lo ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (bbhosts_start) */

loader::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
	    if ((rs = mkpath(pbuf,pr,fn)) >= 0) {
    		cint	vn = 10 ;
		cint	vo = 0 ;
		if ((rs = op->start(vn,vo)) >= 0) {
		    rs = op->loadfile(1,fn) ;
		    c = rs ;
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        } /* end if (file) */
	    } /* end if (mkpath) */
	    rs1 = lm_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (loader::operator) */

int bbhosts_finish(bbhosts *op) noex {
	return vecstr_finish(op) ;
}
/* end subroutine (bbhosts_finish) */

int bbhosts_get(bbhosts *op,int µi,cchar **rpp) noex {
	return vecstr_get(op,µi,rpp) ;
}
/* end subroutine (bbhosts_get) */

int bbhosts_find(bbhosts *op,cchar *s) noex {
	return vecstr_find(op,s) ;
}
/* end subroutine (bbhosts_find) */


/* private subroutines */


