/* getrand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get random data from the UNIX® kernel */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getrand

	Synopsis:
	int getrand(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer
	rlen		length of supplied result buffer

	Returns:
	>=0		returned number of bytes
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucrand.h>
#include	<localmisc.h>

#include	"getrand.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
constexpr T maxval = (compl (T(1) << ((szof(T) * CHAR_BIT) - 1))) ;

template<typename T> local int getrandx(T *p) noex {
    	const T	mv = maxval<T> ;
    	cint	sz = szof(T) ;
	int	rs = SR_FAULT ;
	if (p) {
	    rs = uc_rand(p,sz) ;
	    *p &= mv ;
	}
	return rs ; 
} /* end subroutine-template (getrandx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int getrand(void *rbuf,int rlen)	noex {
	return uc_rand(rbuf,rlen) ;
}
/* end subroutine (getrand) */

int getrandi	(int *p)		noex {
    return getrandx(p) ;
}

int getrandl	(long *p)		noex {
    return getrandx(p) ;
}

int getrandll	(longlong *p)		noex {
    return getrandx(p) ;
}


