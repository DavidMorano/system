/* bvchapters SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a BVI database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvi

	Description:
	This subroutine opens and allows for reading or auditing
	of a BVI (Bible Verse Index) database.

	Synopsis:
	int bvi_open(BVI *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bvchapters.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bvchapters::start(uchar *µap,int µal) noex {
    	int		rs = SR_FAULT ;
	if (µap) {
	    rs = SR_INVALID ;
	    if (µal >= 0) {
	        rs = SR_OK ;
	        ap = µap ;
	        al = µal ;
	        chapidx = 0 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method bvchapters::start) */

int bvchapters::load(int µci,int nv) noex {
    	int		rs = SR_OK ;
	if (nv <= UCHAR_MAX) {
	    if (µci < al) {
		rs = SR_OK ;
	        while (chapidx < µci) {
	            ap[chapidx++] = 0 ;
	        } /* end while */
	        if (chapidx == µci) {
	            ap[chapidx++] = ucharconv(nv) ;
	        } /* end if */
	        µci = chapidx ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} else {
	    rs = SR_RANGE ;
	}
	return (rs >= 0) ? µci : rs ;
} /* end method (bvchapters::load) */


/* private subroutines */

int bvchapters::ifinish() noex {
	return chapidx ;
} /* end method (bvchapters::ifinish) */

bvchapters_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bvchaptersmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bvchapters_co::operator) */


