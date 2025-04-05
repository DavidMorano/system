/* strpack_obj SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* string pack object */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-01-12, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strpack

	Description:
	This object module creates and manages a string storage
	object.  This object is sort of a write-only storage area
	for storing strings.  Strings cannot be deleted from the
	object once they are added (something not commonly needed
	anyway).  Since most string storage requirements do NOT
	need the ability to remove strings once they are entered,
	this object provides a nice optimized manager for handling
	that sort of situation.

	This object is very similar to a STRTAB object (in that
	strings cannot be deleted) but is somewhat more optimized.

	Arguments:
	op		pointer to the strpack object
	<others>

	Returns:
	>=0		the total length of the filled up strpack so far!
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<vechand.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strpack.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int strpack::store(cchar *sp,int sl,cchar **rpp) noex {
	return strpack_store(this,sp,sl,rpp) ;
}

void strpack::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("strpack",rs,"fini-finish") ;
	}
}

int strpack_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case strpackmem_start:
	        rs = strpack_start(op,a) ;
	        break ;
	    case strpackmem_count:
	        rs = strpack_count(op) ;
	        break ;
	    case strpackmem_size:
	        rs = strpack_size(op) ;
	        break ;
	    case strpackmem_finish:
	        rs = strpack_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (strpack_co::operator) */


