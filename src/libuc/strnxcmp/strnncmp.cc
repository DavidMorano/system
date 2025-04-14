/* strnncmp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* compare the minimum common characters of two strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
        Originally written for a convenience so that I do not have to have this
        same function everywhere. How useful is this? I do not know.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnncmp

	Description:
	This subroutine compares two strings but only looks at the
	minimum number of characters that each string has in common
	as determined by the length of each string.

	Synopsis:
	int strnncmp(cchar *s1,int n1,cchar *s2,int n2) noex

	Arguments:
	s1	c-string -1- pointer
	n1	c-string -1- length
	s2	c-string -2- pointer
	n2	c-string -2- length

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strncmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<libutil.hh>
#include	<localmisc.h>

#include	"strnxcmp.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnncmp(cchar *s1,int n1,cchar *s2,int n2) noex {
	int		rc ;
	if (n1 < 0) n1 = xstrlen(s1) ;
	if (n2 < 0) n2 = xstrlen(s2) ;
	{
	    cint	n = min(n1,n2) ;
	    if ((rc = strncmp(s1,s2,n)) == 0) {
	        rc = (n1 - n2) ;
	    }
	}
	return rc ;
}
/* end subroutine (strnncmp) */


