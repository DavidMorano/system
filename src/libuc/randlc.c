/* randlc (Linear Congruent) */
/* lang=C20 */

/* cheap random number generator */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A­D­ Morano
	This is the bad Linear Congruential (LC) RNG from the ACM
	article back in the 80s (I think). This RNG is the LG RNG
	used for the |random()| subroutine in some UNIX®i's when
	used in LC mode.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	You may want to note that this RNG is pretty bad as compared
	with some "good" ones. Don't use this if you can help it!
	The redeeming grace of this RNG (if any) is that it is not
	an object and does not require any persistent state.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<localmisc.h>

#include	"randlc.h"


/* forward references */


/* exported subroutines */

int randlc(int v) noex {
	int		hi, lo ;
	if (v <= 0) v = (31415926 ^ v) ;
	hi = v / 127773 ;
	lo = v % 127773 ;
	v = (16807 * lo) - (2836 * hi) ;
	if (v <= 0) v += INT_MAX ;
	return v ;
}
/* end subroutine (randlc) */


