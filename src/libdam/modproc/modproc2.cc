/* modproc2 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load module names from a C++ source code file */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	modproc

	Notes:
	1. The current output types are:
	name		"<name>"
    	mod		"mod/<name>.ccm"
    	gcm		"gcm.cache/<name>.gcm"
	obj		"<name>.o"
	modobj		"mod/<name>.o"

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX |O_{xx}| */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD |FILE| */
#include	<new>			/* C++ |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modproc ;

import deb ;

/* local defines */

#define CDEBPR(FMT, ...) \
	if_constexpr (f_debug) \
    	debprintf(__func__, FMT __VA_OPT__(,) __VA_ARGS__)


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef vecstr *	vecstrp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int modproc::istart() noex {
    	cnullptr	np{} ;
    	int		rs = SR_NOMEM ;
	CDEBPR("ent\n") ;
    	if (vecstr *vsp ; (vsp = new(nothrow) vecstr) != np) {
	    {
	        vop = vsp ;
	        rs = vsp->start ;
	    }
	    if (rs < 0) {
		delete vsp ;
		vop = nullptr ;
	    } /* end if (error) */
	} /* end if (new-vecstr) */
	CDEBPR("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (modproc::istart) */

int modproc::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (vop)  {
	    vecstr	*vsp = vecstrp(vop) ;
	    rs = SR_OK ;
	    {
		rs1 = vsp->finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete vsp ;
	    } /* end block (memory-release) */
	    vop = nullptr ;
	} /* end if (non-null) */
	CDEBPR("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (modproc::ifinish) */

int modproc::procfile(cchar *fn) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ;
	CDEBPR("ent fn=%s\n",fn) ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		vecstr *vsp = vecstrp(vop) ;
		rs = modprocload(vsp,fn) ;
		c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	CDEBPR("ret rs=%d\n",rs) ;
	return (rs >= 0) ? c : rs ;
} /* end method (modproc::procfile) */

int modproc::procout(FILE *osp,int ot) noex {
        cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	CDEBPR("ent ot=%u\n",ot) ;
	if (osp) {
	    vecstr	*vsp = vecstrp(vop) ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    (void) ot ;
	    CDEBPR("for-before\n") ;
	    for (int i = 0 ; (rs1 = vsp->get(i,&cp)) >= 0 ; i += 1) {
		if (cp) {
	            rs = fprintf(osp,"%s\n",cp) ;
		    c += 1 ;
		}
		if (rs < 0) break ;
	    } /* end for */
	    CDEBPR("for-after rs=%d rs1=%d\n",rs,rs1) ;
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	} /* end if (non-null) */
	CDEBPR("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (modproc::procout) */


/* local subroutines */

void modproc::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("modproc",rs,"fini-finish") ;
	}
} /* end method (modproc::dtor) */

modproc_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case modprocmem_start:
	        rs = op->istart() ;
	        break ;
	    case modprocmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (modproc_co::operator) */


