/* willaddover */
/* lang=C98 */

/* will an addition (long integer) overflow? */
/* version %I% last-modified %G% */


/* revision history:

	= 2012-11-21, David A­D­ Morano
	I took this from some of my previous code.

*/

/* Copyright (c) 2011 David A-D- Morano.  All rights reserved. */

/******************************************************************************

	Name:
	willAddOver

	Description:
	This generic subroutine will determine if an addition will overflow.

	Synopsis:
	int willAddOver(long int n1,long int n2) noex

	Arguments:
	n1		first number to add
	n2		second number to add

	Returns:
	0 (false)	will not overflow
	1 (true)	will overflow

******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"willaddover.h"


int willAddOver(long n1,long n2) noex {
	int	f = FALSE ;
	f = f || ((n1 > 0) && (n2 > 0) && (n1 > (LONG_MAX - n2))) ;
	f = f || ((n1 < 0) && (n2 < 0) && (n1 < (LONG_MIN - n2))) ;
	return f ;
}
/* end subroutine (willAddOver) */


