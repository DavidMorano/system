/* mkcaselower */
/* lang=C20 */

/* change the character case in a c-string to lower-case inplace */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkcaselower

	Description:
	We make a character (an 8-bit entity) out of an integer.
	We do this quite simply.

	Synopsis:
	int mkcaselower(char *rp,int rl) noex

	Arguments:
	rp		source and destination buffer
	rl		length of source string

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<char.h>
#include	<localmisc.h>


/* local defines */


/* exported subroutines */

int mkcaselower(char *rp,int rl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (rp) {
	    rs = SR_OK ;
	    if (rl < 0) rl = INT_MAX ;
	    for (i = 0 ; (i < rl) && *rp ; i += 1) {
	        if (CHAR_ISUC(rp[i])) rp[i] = CHAR_TOLC(rp[i]) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkcaselower) */


