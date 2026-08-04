/* strwildsub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find a substring in a given string w/ wild-card characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This was written because code within the PCS distribution
	uses subroutines that perform this function but which are
	scattered throughout that distribution in different plaecs.
	So I am consolidating this function here to be used by all.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strwildsub

	Description:
	This is a special version of |strstr(3c)| that is modified
	for use in old PCS programs.  If the second c-string argument
	is a substring of the first, we return a pointer to the
	occurance of the second string in the first, othewise we
	return a NULL pointer.  The twist is that we will allow a
	'?' character in the second string to match any character.

	Synopsis:
	int strwildsub(cchar *sp,cchar *sstr) noex

	Arguments:
	sp		c-string to search through
	sstr		c-string to search for in the first string above

	Returns:
	-		sub-string was found at this pointer location
	nullptr		sub-string was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strwildsub.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool gotsubstr(cchar *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwildsub(cchar *sp,cchar *sstr) noex {
    	char		*rp = nullptr ;
	if (sp && sstr) {
	    bool f = false ;
	    while (*sp) {
		if ((f = gotsubstr(sp,sstr))) break ;
		sp += 1 ;
	    } /* end while */
	    if (f) rp = charp(sp) ;
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strwildsub) */


/* local subroutines */

local bool mat(int ch,int ch_s) noex {
    	return ch && ((ch == ch_s) || (ch_s == '?')) ;
} /* end subroutine (mat) */

local bool gotsubstr(cchar *sp,cchar *sstr) noex {
    	bool f = false ;
	while (*sstr) {
	    if (! ((f = mat(*sp++,*sstr++)))) break ;
	} /* end while */
    	return f ;
} /* end subroutine (gotsubstr) */


