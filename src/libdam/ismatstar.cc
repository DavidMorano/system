/* ismatstar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* see if a string matches a "starmat" ('*') expresssion */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-17, David A­D­ Morano
	This subroutine was coded up to handle some PCS wild-card
	type matching.  Do I have it correct?  Unlike the stupid
	standard UNIX® REGEX stuff, this subroutine is entirely
	reentrant!

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ismatstar

	Description:
	This subroutine does a simple wild-card pattern match.  The
	only meta-pattern character allowed is the asterisk!  Further,
	we only allow one asterisk in the whole pattern expression!
	Sorry, live with it.

	Synopsis:
	bool ismatstar(cchar *se,cchar *s) noex

	Arguments:
	se		pattern to match against
	s		string to test for a match

	Returns:
	TRUE		match
	FALSE		no match

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |str{x}(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"ismatstar.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool ismatstar(cchar *se,cchar *s) noex {
	bool		f ;
	if (cchar *tp ; (tp = strchr(se,'*')) != nullptr) {
	    f = false ;
	    if (int sl ; strncmp(s,se,(tp - se)) == 0) {
	        cint	sl1 = lenstr(s) ;
	        cint	sl2 = lenstr(se) ;
	        tp += 1 ;
	        sl = (se + sl2) - tp ;
	        f = (strncmp((s + sl1 - sl),tp,sl) == 0) ;
	    } /* end if */
	} else {
	    f = (strcmp(se,s) == 0) ;
	}
	return f ;
}
/* end subroutine (ismatstar) */


