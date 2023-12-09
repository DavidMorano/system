/* hasMeAlone */
/* lang=C20 */

/* test whether a string has only certain chacters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasMeAlone

	Description:
	We test whether the given string consists only of those
	characters that symbolically represent the "current" user.

	Synopsis:
	int hasMeAlone(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int hasMeAlone(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sl < 0) strlen(sp) ;
	if (sl == 1) {
	    const int	ch = MKCHAR(*sp) ;
	    switch (ch) {
	    case '+':
	    case '-':
	    case '!':
		f = true ;
		break ;
	    } /* end switch */
	} /* end if */
	return f ;
}
/* end subroutine (hasMeAlone) */


