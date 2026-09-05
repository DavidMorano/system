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

  	Object:
	paramopt

	Description:
	This is very similar to |paramopt_loadu(3dam)| but it allows
	for a special separator character between the key and the
	data values; the special separator character is ':'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<six.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"paramopt.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import paramopt_mag ;

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
} /* end method */

int paramopt::loads(cchar *name,cchar *sp,int sl) noex {
	return paramopt_loads(this,name,sp,sl) ;
} /* end method */

int paramopt::load(cchar *name,cchar *vbuf,int vlen) noex {
	return paramopt_load(this,name,vbuf,vlen) ;
} /* end method */

int paramopt::loaduniq(cchar *name,cchar *vp,int vl) noex {
	return paramopt_loaduniq(this,name,vp,vl) ;
} /* end method */

int paramopt::loadone(cchar *sp,int sl) noex {
	return paramopt_loadone(this,sp,sl) ;
} /* end method */

int paramopt::havekey(cchar *name) noex {
	return paramopt_havekey(this,name) ;
} /* end method */

int paramopt::haveval(cchar *key,cchar *vp,int vl) noex {
	return paramopt_haveval(this,key,vp,vl) ;
} /* end method */

int paramopt::curbegin(PO_CUR *curp) noex {
	return paramopt_curbegin(this,curp) ;
} /* end method */

int paramopt::curend(PO_CUR *curp) noex {
	return paramopt_curend(this,curp) ;
} /* end method */

int paramopt::curenumkey(PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curenumkey(this,curp,rpp) ;
} /* end method */

int paramopt::curenumval(cchar *key,PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curenumval(this,key,curp,rpp) ;
} /* end method */

int paramopt::curfetch(cchar *key,PO_CUR *curp,cchar **rpp) noex {
	return paramopt_curfetch(this,key,curp,rpp) ;
} /* end method */

int paramopt_count(paramopt *op) noex {
    	int		rs = SR_NOTOPEN ;
	if (op->head) {
	    rs = 0 ;
	}
	return rs ;
} /* end subroutine */

int paramopt::countvals(cchar *key) noex {
	return paramopt_countvals(this,key) ;
} /* end method */

void paramopt::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("paramopt",rs,"fini-finish") ;
	}
} /* end method (paramopt::dtor) */

paramopt::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (head) ylikely {
	    rs = paramopt_count(this) ;
	}
	return rs ;
} /* end method (paramopt::operator) */

paramopt_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
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
} /* end method (paramopt_co::operator) */


