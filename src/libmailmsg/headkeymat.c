/* headkeymat SUPPORT */
/* lang=C++20 */

/* match on header keys */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Module was originally written.  This was written as part of
	the PCS mailer code cleanup!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Given a header key name 'key' such as "Subject", find if
	it is present in the user supplied string given as 'buf'.
	Return 0 if there is no match, else we return the character
	position of the start of the header value string.  The match
	is case independent.

	Synopsis:
	int headkeymat(cc *key,cc *sp,int sl) noex

	Arguments:
	key		key name to test for
	sp		c-string to match pointer
	sl		c-string to match length

	Returns:
	>0		the key was found and the position of the value (not 
			the key) in the user supplied string under test is 
			returned
	==0		the key was not found
	<0		not possible (hopefully)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>
#include	<char.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int headkeymat(cchar *key,cchar *buf,int buflen) noex {
	int		bl = buflen ;
	cchar	*bp = buf ;
	cchar	*kp = key ;

	if (buflen < 0) {

	    while (*kp && (*kp != ':')) {

	        if (CHAR_TOLC(*bp) != CHAR_TOLC(*kp))
	            return 0 ;

	        kp += 1 ;
	        bp += 1 ;

	    } /* end while */

	    while (CHAR_ISWHITE(*bp))
	        bp += 1 ;

	} else {

	    while (*kp && (*kp != ':') && (bl > 0)) {

	        if (CHAR_TOLC(*bp) != CHAR_TOLC(*kp))
	            return 0 ;

	        kp += 1 ;
	        bp += 1 ;
	        bl -= 1 ;

	    } /* end while */

	    while (CHAR_ISWHITE(*bp) && (bl > 0)) {
	        bp += 1 ;
	        bl -= 1 ;
	    }

	} /* end if */

	if (*bp++ != ':')
	    return 0 ;

	if (buflen < 0) {

	    while (CHAR_ISWHITE(*bp))
	        bp += 1 ;

	} else {

	    while (CHAR_ISWHITE(*bp) && (bl > 0)) {
	        bp += 1 ;
	        bl -= 1 ;
	    }

	}

	return (bp - buf) ;
}
/* end subroutine (headkeymat) */


