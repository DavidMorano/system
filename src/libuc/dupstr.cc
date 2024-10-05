/* dupstr SUPPORT */
/* lang=C++20 */

/* create a (writable) duplicate of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	dupstr

	Description:
	This object module (DUPSTR) creates a writable duplicate
	of the given string.  Note: for a simple non-writable
	(constant) copy of a string, check out the NULSTR object.

	Synopsis:
	int dupstr_start(DUPSTR *op,cchar *sp,int sl,char **rpp) noex
	int dupstr_finish(DUPSTR *op) noex

	Arguments:
	op		object pointer
	sp		source c-string to copy (pointer)
	sl		source c-string to copy (length)
	rpp		pointer to hold a copy of the source string

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"dupstr.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int dupstr_start(dupstr *op,cchar *sp,int sl,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		cl = 0 ;
	if (op && sp && rpp) {
	    rs = SR_OK ;
	    op->as = nullptr ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl > DUPSTR_SHORTLEN) {
	        cint	ssize = (sl+1) ;
	        if (char *bp{} ; (rs = uc_malloc(ssize,&bp)) >= 0) {
	            cl = strwcpy(bp,sp,sl) - bp ;
	            *rpp = bp ;
	            op->as = bp ;
	        } /* end if (m-a) */
	    } else {
	        *rpp = op->buf ;
	        cl = strwcpy(op->buf,sp,sl) - op->buf ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (dupstr_start) */

int dupstr_finish(dupstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->as) {
	        rs1 = uc_free(op->as) ;
	        if (rs >= 0) rs = rs1 ;
	        op->as = nullptr ;
	    }
	    op->buf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dupstr_finish) */


/* local subroutines */

int dupstr::start(cchar *sp,int sl,char **rpp) noex {
	return dupstr_start(this,sp,sl,rpp) ;
}

void dupstr::dtor() noex {
	cint	rs = int(finish) ;
	if (rs < 0) {
	    ulogerror("dupstr",rs,"fini-finish") ;
	}
}

dupstr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case dupstrmem_finish:
	        rs = dupstr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (dupstr_co::operator) */


