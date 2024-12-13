/* minmaxelem HEADER */
/* encoding=ISO8859-1 */
/* lang=C++98 */

/* calculate the minimum and maximum at the same time */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2018-09-15, David A­D­ Morano
        This was separated out from where it was to make a stand-alone
        (exported) subroutine.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	minmaxelem

	Description:
	We find both the minimum and the maximum element in a range
	of a container.

*******************************************************************************/

#ifndef	MINMAXELEM_INCLUDE
#define	MINMAXELEM_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<vector>
#include	<usystem.h>


template<typename I,typename T>
pair<T,T> minmaxelem(const I &bit,const I &eit) noex {
	pair<T,T>	res ;
	I		it ;
	int		minval = INT_MAX ;
	int		maxval = 0 ;
	for (it = bit ; it != eit ; it += 1) {
	    int	e = (*it) ;
	    if (e < minval) minval = e ;
	    if (e > maxval) maxval = e ;
	} /* end for */
	res.first = minval ;
	res.second = maxval ;
	return res ;
}
/* end subroutine-template (minmaxelem) */


#endif /* __cplusplus */
#endif /* MINMAXELEM_INCLUDE */


