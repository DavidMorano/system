/* hasduplicate MODULE (primary module interface) */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* does the given array of elements have duplicate entries */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-10-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasduplicate

	Description:
	This subroutine checks if the given array of elements (type
	'T') has duplicate entries.

	Synopsis:
	int hasduplicate(const int *sp,int sl) noex

	Arguments:
	sp		source array of integers
	sl		length of source array

	Returns:
	<0		error
	==0		no duplicates
	==1		found a duplicate entry

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<unordered_set>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

export module hasduplicate ;

/* local defines */


/* external subroutines */


/* external variables */


/* forwards references */


/* local variables */


/* exported variables */


/* exported subroutines */

export {
    template<typename T> bool hasduplicate(const T *sp,int sl) noex {
	typedef typename std::unordered_set<T>::iterator iter_t ;
	std::unordered_set<T>	visited ;
	int		rs = SR_OK ;
	int		f = false ;
	if (sl > 1) {
	    try {
	        iter_t itend = visited.end() ;
	        for (int i = 0 ; i < sl ; i += 1) {
	            if (visited.find(*sp) != itend) {
		        f = true ;
		        break ;
		    } else {
		        visited.insert(*sp) ;
		    }
	        } /* end for */
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	} /* end if (needed more work) */
	return (rs >= 0) ? f : rs ;
    } /* end subroutine (hasduplicate) */
} /* end export */


