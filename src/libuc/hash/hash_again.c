/* hash_again */
/* lang=C20 */

/* create a hash from an exiting hash */
/* version %I% last-modified %G% */

#define	CF_RANDLC	1		/* use 'randlc(3dam)' */

/* revision history:

	= 2001-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hash_again

	Description:
	This subroutine takes an old (existing) hash and creates a
	new one based on some other factors passed.

	Synopsis:
	uint hashagain(uint ohash,int c,int nskip) noex

	Arguments:
	ohash		hash to use in calculation
	c		re-hash count 
	nskip		number of times to allow skipping around

	Returns:
	value		the hash value (unsigned)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* local defines */

#define	NROT	-5			/* rotation amount (in bits) */


/* external subroutines */

extern uint	urotate(uint,int) noex ;

extern int	randlc(int) noex ;


/* forward references */


/* exported subroutines */

uint hash_again(uint ohash,int c,int nskip) noex {
	uint		nhash = ohash ;
	if (c < nskip) {

#if	CF_RANDLC
	    nhash = randlc(ohash + c) ;
#else
	    nhash = urotate(ohash,NROT) + c ;
#endif /* CF_RANDLC */

	    if (ohash == nhash) {
		nhash = (ohash + 1) ;
	    }
	} else {
	    nhash = (ohash + 1) ;
	}
	return nhash ;
}
/* end subroutine (hash_again) */


