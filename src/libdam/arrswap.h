/* arrswap HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a seed value for use in initializing a true RNG */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	arrswap{x}

	Description:
	We swap two elements in an array of integers.

	Synopsis:
	void arrswap{x}(int *a,int i1,int i2) noex

	Arguments:
	{x}	one of: 'i' 'l' 'll'
	a	array of integers
	i1	element to swap
	i2	element to swap

	Returns:
	-		none (void)

*******************************************************************************/

#ifndef	ARRSWAP_INCLUDE
#define	ARRSWAP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin
extern void	arrswapi(int *,int,int) noex ;
extern void	arrswapl(long *,int,int) noex ;
extern void	arrswapll(longlong *,int,int) noex ;
EXTERNC_end


#endif /* ARRSWAP_INCLUDE */


