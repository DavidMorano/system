/* cmpstrnn SUPPORT */
/* charset=ISO8859-1 */
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
	cmpstrnn

	Description:
	This subroutine compares two strings but only looks at the
	minimum number of characters that each string has in common
	as determined by the length of each string.

	Synopsis:
	int cmpstrnn(cchar *s1,int n1,cchar *s2,int n2) noex

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ucmpx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cmpstrnn.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

int cmpstrnn(cchar *s1,int n1,cchar *s2,int n2) noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    if (n1 < 0) n1 = lenstr(s1) ;
	    if (n2 < 0) n2 = lenstr(s2) ;
	    if (cint n = min(n1,n2) ; (rc = cmpstrn(s1,s2,n)) == 0) {
	        rc = (n1 - n2) ;
	    } /* end */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (cmpstrnn) */


