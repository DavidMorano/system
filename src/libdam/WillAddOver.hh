/* willAddOver */
/* lang=C++20 */

/* will an addition (generic) overflow? */
/* version %I% last-modified %G% */


/* revision history:

	= 2012-11-21, David A­D­ Morano
	I took this from some of my previous code.

*/

/* Copyright � 2012 David A-D- Morano.  All rights reserved. */

/******************************************************************************

	Name:
	willAddOver

	Description:
	This generic subroutine will determine if an addition will
	overflow.

	Synopsis:
	int willAddOver(T n1, T n2)

	Arguments:
	n1		first number to add
	n2		second number to add

	Returns:
	0 (false)	will not overflow
	1 (true)	will overflow

******************************************************************************/

#ifndef	WILLADDOVER_INCLUDE
#define	WILLADDOVER_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<valuelimits.hh>
#include	<localmisc.h>


template<T>
int willAddOver(T n1,T n2) noex {
	const int	s = sizeof(T) ;
	T		min = 0 ;
	T		max = 0 ;
	int		f = FALSE ;
	switch (s) {
	case 1:
	    min = SCHAR_MIN ;
	    max = SCHAR_MAX ;
	    break ;
	case 2:
	    min = SHRT_MIN ;
	    max = SHRT_MAX ;
	    break ;
	case 4:
	    min = INT_MIN ;
	    max = INT_MAX ;
	    break ;
	case 8:
	    min = LONG_MIN ;
	    max = LONG_MAX ;
	    break ;
	case 16:
	    min = valuelimit.llmin ;
	    max = valuelimit.llmax ;
	    break ;
	} /* end switch */
	if (max != 0) {
	    f = f || (n1 > 0) && (n2 > 0) && (n1 > (max - n2)) ;
	    f = f || (n1 < 0) && (n2 < 0) && (n1 < (min - n2)) ;
	}
	return f ;
}
/* end subroutine (willAddOver) */


#endif /* WILLADDOVER_INCLUDE */


