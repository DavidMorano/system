/* hasdupplicate MODULE (primary module interface) */
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
	int hasdupplicate(const int *sp,int sl) noex

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
#include	<cstring>
#include	<unordered_set>
#include	<usystem.h>
#include	<localmisc.h>

export module hasduplicate ;

/* local defines */


/* external subroutines */


/* forwards references */


/* exported variables */


/* exported subroutines */

export {
    template<typename T> bool hasduplicate(const T *sp,int sl) noex {
	typename std::unordered_set<T>	visited ;
	int		rs = SR_OK ;
	int		f = false ;
	if (sl > 1) {
	    typename std::unordered_set<T>::iterator	end = visited.end() ;
	    for (int i = 0 ; i < sl ; i += 1) {
	        if (visited.find(*sp) != end) {
		    f = true ;
		    break ;
		} else {
		    visited.insert(*sp) ;
		}
	    } /* end for */
	} /* end if (needed more work) */
	return (rs >= 0) ? f : rs ;
    } /* end subroutine (hasduplicate) */
} /* end export */


