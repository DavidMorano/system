/* kvparse1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* key-value parsing */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	kvparse

	Description:
	Key-Value Parse.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<fieldterminit.hh>	/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

module kvparse ;

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr fieldterminit		kvt(" #=") ;


/* exported variables */


/* exported subroutines */

int kvparse::extract(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	assert(sp) ;
	initvars() ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) {
	        cchar	*fp ;
	        if (int fl ; (fl = fsb.get(kvt.terms,&fp)) > 0) {
	            if (fsb.term != '#') {
	        	cchar	*f2p ;
	        	if (int f2l ; (f2l = fsb.get(kvt.terms,&f2p)) >= 0) {
			    keyp = fp ;
			    valp = f2p ;
			    keyl = fl ;
			    vall = f2l ;
			    rv = fl ; /* <- key-length */
			} /* end if */
	            } /* end if (got unemcumbered key) */
	        } /* end if */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (kvparse::extract) */


