/* cheaprand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* cheap random number generator */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A­D­ Morano
	I made this subroutine to make a random number (scramble
	up one really) for 64-bit integers. I am not aware of any
	existing random number generator (scrambler) for 64-bit
	integers so far. I suppose that once all machines are 64
	bits (and they adhere to the LP64 model) all 'long's will
	be 64 bits without us even knowing about it! Whatever.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a cheapy random number generator. There is no theory
	or proofs that this is a "good" RNG. Although this RNG is
	part of a cryptographically strong messages system, this
	RNG in itself is never used as the main component.

	The |randmac()| subroutine below is a 32 bit RNG that is
	based loosely on the well-performing Multiphy With Carry
	type generators. However, and note carefully, the '|andmac()|
	subroutine is NOT of that type. A MWC adds in the carry
	from the PREVIOUS multiply where the subroutine below adds
	in the carry from the CURRENT multiply. It appears to perform
	well in RNG tests.

	The |cheaprand()| subroutine is a combination of two of the
	|randmac()| RNGs. Again, there is no theory or proof that
	this is a good RNG either.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<usystem.h>

#include	"cheaprand.h"


/* local defines */


/* forward references */

static uint	randmac(uint) noex ;


/* exported variables */


/* exported subroutines */

ulong cheaprand(ulong ow) noex {
	ulong		result ;
	ulong		lo = (ow >> 00) ;
	ulong		hi = (ow >> 32) ;
	uint		ihi, ilo ;
	uint		ehi, elo ;
	ihi = (uint) hi ;
	ilo = (uint) lo ;
	ehi = randmac(ihi) + ilo ;
	elo = randmac(ilo) + ihi ;
	hi = (ulong) ehi ;
	lo = (ulong) elo ;
	result = (hi << 32) | lo ;
	return result ;
}
/* end subroutine (cheaprand) */


/* local subroutines */

static uint randmac(uint v) noex {
	ulong		x = (ulong) v ;
	ulong		sum, prod ;
	ulong		a = 1967773755 ;
	uint		nv ;
	prod = a * x ;
	sum = prod + (prod >> 32) ;
	nv = (uint) sum ;
	return nv ;
}
/* end subroutine (randmac) */


