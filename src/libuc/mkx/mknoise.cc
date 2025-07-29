/* mknoise SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve process ID related noise from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This is just a quick hack to get some additional noise from
	a UNIX® system that is fairly portable (a big problem).  I
	really failed here because the most noise we get is from
	the presence of the current processes on the machine and
	this is very non-portable on systems not running System V.

	= 2024-05-08, David A­D­ Morano
	I wanted to get rid of the dependency on the |sha1| object.
	But instead I realized that this whole subroutine can be
	written in terms of the |uc_rand(3uc)| subroutine.  So I
	did it (rewrote this whole thing).  See what I did below.

*/

/* Copyright © 2002,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mknoise

	Description:
	This subroutine acquires some of the noise from the system.
	Currently (unlike previously) this subroutine just calls
	the system |uc_rand(3uc)| subroutine to get some high quality
	random data, rather than trying to fabricate something itself
	(like it did previously).

	Synopsis:
	int mknoise(uint *a,int n) noex

	Arguments:
	a	array of (unsigned) integers to receive the noise
	n	number of integers supplied by caller

	Returns:
	>=0	number of integers containing random data being returned
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"mkx.h"


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

int mknoise(uint *a,int n) noex {
	int		rs = SR_FAULT ;
	if (a) ylikely {
	    rs = SR_INVALID ;
	    if (n > 0) ylikely {
	        cint	sz = szof(uint) ;
	        if ((rs = uc_rand(a,(n * sz))) >= 0) ylikely {
		    n = (rs / sz) ;
		}
	    }
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mknoise) */


