/* paramopt_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load one parameter (similar to 'paramopt_loadu(3dam)' but special) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This is very similar to |paramopt_loadu(3dam)| but it allows
	for a special separator character between the key and the
	data values; the special separator character is ':'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<nulstr.h>
#include	<six.h>
#include	<char.h>
#include	<localmisc.h>

#include	"paramopt.h"

import libutil ;

/* local defines */

#define	PO		paramopt
#define	PO_CUR		paramopt_cur
#define	PO_NAME		paramopt_name
#define	PO_VAL		paramopt_val


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int paramopt::loadu(cchar *sp,int sl) noex {
	return paramopt_loadu(this,sp,sl) ;
}

int paramopt::loads(cchar *name,cchar *sp,int sl) noex {
	return paramopt_loads(this,name,sp,sl) ;
}

int paramopt::load(cchar *name,cchar *vbuf,int vlen) noex {
	return paramopt_load(this,name,vbuf,vlen) ;
}

int paramopt::loaduniq(cchar *name,cchar *vp,int vl) noex {
	return paramopt_loaduniq(this,name,vp,vl) ;
}

int paramopt::loadone(cchar *sp,int sl) noex {
	return paramopt_loadone(this,sp,sl) ;
}

int paramopt::havekey(cchar *name) noex {
	return paramopt_havekey(this,name) ;
}

int paramopt::haveval(cchar *key,cchar *vp,int vl) noex {
	return paramopt_haveval(this,key,vp,vl) ;
}

int paramopt::curbegin(PO_CUR *curp) noex {
	return paramopt_curbegin(this,curp) ;
}

int paramopt::curend(PO_CUR *curp) noex {
	return paramopt_curend(this,curp) ;
}

int paramopt::curenumkey(PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curenumkey(this,curp,rpp) ;
}

int paramopt::curenumval(cchar *key,PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curenumval(this,key,curp,rpp) ;
}

int paramopt::curfetch(cchar *key,PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curfetch(this,key,curp,rpp) ;
}

int paramopt_count(paramopt *op) noex {
    	int		rs = SR_NOTOPEN ;
	if (op->head) {
	    rs = 0 ;
	}
	return rs ;
}

int paramopt::countvals(cchar *key) noex {
	return paramopt_countvals(this,key) ;
}

void paramopt::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("paramopt",rs,"fini-finish") ;
	}
}

paramopt::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (head) {
	    rs = paramopt_count(this) ;
	}
	return rs ;
}

paramopt_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case paramoptmem_start:
	        rs = paramopt_start(op) ;
	        break ;
	    case paramoptmem_count:
	        rs = paramopt_count(op) ;
	        break ;
	    case paramoptmem_incr:
	        rs = paramopt_incr(op) ;
	        break ;
	    case paramoptmem_finish:
	        rs = paramopt_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (paramopt_co::operator) */


