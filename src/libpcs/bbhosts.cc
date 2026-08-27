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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |ulogerror(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<vecpstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bbhosts.hh"


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

template<typename ... Args>
local inline int bbhosts_magic(bbhosts *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BBHOSTS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (bbhosts_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int bbhosts_start(bbhosts *op,cchar *pr,cchar *bbhostfn) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if (op && pr && bbhostfn) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0] && bbhostfn[0]) ylikely {
		if (loader lo(op,pr,bbhostfn) ; (rs = lo) >= 0) ylikely {
		    c = rs ;
		    op->magval = BBHOSTS_MAGIC ;
		} /* end if (loader) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bbhosts_start) */

int bbhosts_finish(bbhosts *op) noex {
    	int		rs ;
	if ((rs = bbhosts_magic(op)) >= 0) ylikely {
	    rs = op->vecpstr::finish ;
	    op->magval = 0 ;
	} /* end if (pcsunodes_magic) */
	return rs ;
} /* end subroutine (bbhosts_finish) */

int bbhosts_get(bbhosts *op,int µi,cchar **rpp) noex {
    	int		rs ;
	if ((rs = bbhosts_magic(op)) >= 0) ylikely {
	    rs = op->vecpstr::get(µi,rpp) ;
	} /* end if (pcsunodes_magic) */
	return rs ;
} /* end subroutine (bbhosts_get) */

int bbhosts_find(bbhosts *op,cchar *s) noex {
    	int		rs ;
	if ((rs = bbhosts_magic(op,s)) >= 0) ylikely {
	    rs = op->vecpstr::find(s) ;
	} /* end if (pcsunodes_magic) */
	return rs ;
} /* end subroutine (bbhosts_find) */

int bbhosts_count(bbhosts *op) noex {
    	int		rs ;
	if ((rs = bbhosts_magic(op)) >= 0) ylikely {
    	    rs = op->vecpstr::count ;
	} /* end if (pcsunodes_magic) */
	return rs ;
} /* end subroutine */

int bbhosts_audit(bbhosts *op) noex {
    	int		rs ;
	if ((rs = bbhosts_magic(op)) >= 0) ylikely {
    	    rs = op->vecpstr::audit ;
	} /* end if (pcsunodes_magic) */
	return rs ;
} /* end subroutine */


/* private subroutines */

loader::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
	    if ((rs = mkpath(pbuf,pr,fn)) >= 0) {
    		cint	vn = 10 ;
		cint	vo = 0 ;
		if ((rs = op->vecpstr::start(vn,vo)) >= 0) {
		    rs = op->vecpstr::loadfile(1,pbuf) ;
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

int bbhosts::start(cchar *pr,cchar *bbhostfn) noex {
	return bbhosts_start(this,pr,bbhostfn) ;
} /* end method (bbhosts::start) */

int bbhosts::get(int µi,cchar **rpp) noex {
	return bbhosts_get(this,µi,rpp) ;
} /* end method (bbhosts::get) */

int bbhosts::find(cchar *s) noex {
	return bbhosts_find(this,s) ;
} /* end method (bbhosts::find) */

void bbhosts::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bbhosts",rs,"fini-finish") ;
	}
} /* end method (bbhosts::dtor) */

bbhosts::operator int () noex {
    	return count ;
} /* end method (bbhosts::operator) */

bbhosts_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bbhostsmem_count:
	        rs = bbhosts_count(op) ;
	        break ;
	    case bbhostsmem_audit:
	        rs = bbhosts_audit(op) ;
	        break ;
	    case bbhostsmem_finish:
	        rs = bbhosts_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bbhosts_co::operator) */


