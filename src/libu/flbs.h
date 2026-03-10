/* flbs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* Find-Last-Bit-Set (FLBS) in an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	flbs{x}

	Description:
	Find the last bit set in an integer.

	Synopsis:
	template<integral T> int flbs{x}(T v) noex
	int flbs{x}(X v) noex

	Arguments:
	{x}		'int', 'long', 'longlong'
	v		supplied integer value

	Returns:
	-               the last bit that was set

******************************************************************************/

#ifndef	FLBS_INCLUDE
#define	FLBS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	flbsi(int) noex ;
extern int	flbsl(long) noex ;
extern int	flbsll(longlong) noex ;

EXTERNC_end


#endif /* FLBS_INCLUDE */


