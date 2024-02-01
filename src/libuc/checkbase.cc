/* checkbase SUPPORT */
/* lang=C++20 */

/* test whether a string is composed of all characters of a given base */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	checkbase

	Description:
	This subroutine is mostly used as a utility function to
	determine (verify) that all of the characters in a counted
	string are consistent to be be used as digits within the
	supplied numeric base.

	Synopsis:
	int checkbase(cchar *sp,int sl,int b) noex

	Arguments:
	sp		string to test
	sl		length of strin to test
	b		base to check against

	Returns:
	>=0		OK
	<0		bad

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<varnames.hh>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	hasallbase(const char *,int,int) ;
}


/* external variables */


/* local structures */


/* forward references */

extern "C" {
    extern int checkbase(cchar *,int,int) noex ;
}


/* local variables */

static const int	maxbase = strlen(varname.digtab) ;


/* exported subroutines */

int checkbase(cchar *sp,int sl,int b) noex {
	int		rs = SR_INVALID ;
	if ((b >= 2) && (b <= maxbase)) {
	    rs = (hasallbase(sp,sl,b)) ? SR_OK : SR_DOM ;
	}
	return rs ;
}
/* end subroutine (checkbase) */


