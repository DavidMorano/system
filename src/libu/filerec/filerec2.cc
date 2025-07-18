/* filerec2 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filerec

	Description:
	This object implements a set (an un-ordered set) with a key
	consisting of device and inode numbers.

	Symopsis:
	int filerec_start(int n = 0) noex

	Arguments:
	n		suggested starting length

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<utility>		/* |pair(3c++)| */
#include	<unordered_set>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<localmisc.h>

module filerec ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines (methods) */

int filerec::curbegin(filerec_cur *curp) noex {
	int		rs = SR_NOTOPEN ;
	if (setp && curp) ylikely {
	    rs = SR_OK ;
	    curp->it = setp->begin() ;
	} /* end if (valid) */
	return rs ;
} /* end method (filerec::curbegin) */

int filerec::curend(filerec_cur *curp) noex {
	int		rs = SR_NOTOPEN ;
	if (setp && curp) ylikely {
	    rs = SR_OK ;
	    curp->it = setp->end() ;
	} /* end if (valid) */
	return rs ;
} /* end method (filerec::curend) */

int filerec::curenum(filerec_cur *curp,filerec_ent *ep) noex {
	int		rs = SR_NOTOPEN ;
	if (setp && curp && ep) ylikely {
	    stype::iterator ite = setp->end() ;
	    rs = SR_NOTFOUND ;
	    if (curp->it != ite) {
		*ep = *curp->it++ ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end method (filerec::curenum) */


