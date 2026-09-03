/* newobjsub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* new-object-subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	These routines are used when the caller wants to store a COPY
	of the passed string data into a vector.  These routines will
	copy and store the copied data in the list.  The advantage is
	that the caller does not have to keep the orginal data around
	in order for the list data to be accessed later.  String data
	(unlike "element" data) can not contain NULL characters/bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdckdint>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ucmem.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"newobj.h"

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* local namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

void *newobjsub(int ne,int esz) noex {
	void		*rp = nullptr ;
	int		rs = SR_INVALID ;
	if ((ne > 0) && (esz > 0)) ylikely {
	    rs = SR_TOOBIG ;
	    if (int sz{} ; ckd_mul(&sz,ne,esz) == false) ylikely {
	        if (void *p ; (rs = mem.mall(sz,&p)) >= 0) ylikely {
	            rp = p ;
	        } /* end if (memory-acquire) */
	    } /* end if (no-overflow) */
	} /* end if (valid) */
	if (rs < 0) {
	    ulogerror("newobjsub",rs,"allocation") ;
	} /* end if (error) */
	return rp ;
} /* end subroutine (newobjsub) */


