/* vecelem_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector element-list operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecelem

	Description:
	These routines are used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain NUL
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vecelem.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* forward references */


/* local variables */


/* exported variables */

int vecelem::start(int µesz,int µvn,int µvo) noex {
	return vecelem_start(this,µesz,µvn,µvo) ;
}

int vecelem::add(cvoid *ep) noex {
	return vecelem_add(this,ep) ;
}

int vecelem::addlist(cvoid *ep,int el) noex {
	return vecelem_addlist(this,ep,el) ;
}

int vecelem::adduniq(cvoid *ep) noex {
	return vecelem_adduniq(this,ep) ;
}

int vecelem::sort(vecelem_vcmp vcf) noex {
	return vecelem_sort(this,vcf) ;
}

int vecelem::get(int ai,void *vrp) noex {
	return vecelem_get(this,ai,vrp) ;
}

int vecelem::getval(int ai,void *vrp) noex {
	return vecelem_getval(this,ai,vrp) ;
}

int vecelem::getvec(void **rpp) noex {
	return vecelem_getvec(this,rpp) ;
}

int vecelem::del(int ai) noex {
	return vecelem_del(this,ai) ;
}

void vecelem::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecelem",rs,"fini-finish") ;
	}
} /* end method (vecelem::dtor) */

vecelem::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (n > 0) {
	    rs = c ;
	}
	return rs ;
} /* end method (vecelem::operator) */

vecelem_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case vecelemmem_count:
	        rs = vecelem_count(op) ;
	        break ;
	    case vecelemmem_delall:
	        rs = vecelem_delall(op) ;
	        break ;
	    case vecelemmem_extent:
	        rs = vecelem_extent(op) ;
	        break ;
	    case vecelemmem_audit:
	        rs = vecelem_audit(op) ;
	        break ;
	    case vecelemmem_finish:
	        rs = vecelem_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecelem_co::operator) */


