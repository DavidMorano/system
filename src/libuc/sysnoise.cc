/* sysnoise SUPPORT */
/* lang=C++20 */

/* retrieve noise from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	I first made this up to get some additional noise from a
	UNIX® system and which is coded in a fairly portable way
	(a big problem).

	= 2024-02-27, David A­D­ Morano
	This whole subroutine is being changed (about to do it).
	This was originally written before random bits were readily
	available.  The world has changed since 2002 (if this was
	even originally written as late as that).  I am wiping this
	old crap totally clean, and writing this to use |uc_rand(3uc)|
	exclusively.  For those messed-up operating systems that
	do not natively provide |getrandom(3c)| -- like messed-up
	Darwin (MacOS) -- then we use the hacked version of
	|getrandom(3c)| that was written for it as a stop-gap
	measure.  Oh, before I leave here, I just want to say that
	it would ave been nice if Apple Darwin had support for
	|getrandom(3c)| natively.

*/

/* Copyright © 2002,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysnoise

	Description:
	This subroutine acquires the process noise in the system.
	It basically just looks at the process IDs that exist at
	the moment of the call and returns a random sequence of
	data bases on that.

	Synopsis:
	int sysnoise(uchar *a,int alen) noex

	Arguments:
	a	buffer receive the noise
	alen	length (in bytes) of buffer supplied by caller

	Returns:
	>=0	bytes of noise returned
	<0	error code (system-return)

	Important Note:
	There is great danger in this subroutine!  What if the
	sub-process starts up OK but then never writes anything
	into the pipe AND it also just hangs there indefinitely?
	Well, you might say "how can that happen?"  "Even if it
	does not write anything, will not the sub-process eventually
	exit and thereby close its end of the pipe?"  Well, my
	response, is, it has happened!  Remember that we are trying
	to run 'vmstat -s'!  Who knows what weirdo locks it has to
	acquire that might hang rather indefinitely?  That is exactly
	what was observed, it was hanging for days on some system-lock
	of some sort!

	Updated-Note (2024-02-27):
	I totally gutted this old crap and am just writing this afresh
	in terms of using |getrandom(3c)|.  The old version of this
	subroutine is now in the obsolete-source-code storage area.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"sysnoise.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sysnoise(uchar *a,int alen) noex {
	return uc_rand(a,alen) ;
}
/* end subroutine (sysnoise) */


