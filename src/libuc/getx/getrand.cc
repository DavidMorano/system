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

	Group:
	getrand
	getrandi
	getrandl
	getrandll

	Synopsis:
	int getrand	(char *rbuf,int rlen)	noex
	int getrandi	(int *p)		noex
	int getrandl	(long *p)		noex
	int getrandll	(longlong *p)		noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	p		pinter to integer result variable

	Returns:
	>=0		succes or returned number of bytes
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucrand.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

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
} /* end subroutine (getrand) */

int getrandi	(int *p)		noex {
    return getrandx(p) ;
} /* end */

int getrandl	(long *p)		noex {
    return getrandx(p) ;
} /* end */

int getrandll	(longlong *p)		noex {
    return getrandx(p) ;
} /* end */


