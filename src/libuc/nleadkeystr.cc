/* nleadkeystr SUPPORT */
/* lang=C++20 */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nleadkeystr

	Description:
        Check that the given string matches (case independently) the LEADING
        part of some string in the given array of strings. If we get a match, we
        return the number of characters matched. If we do not match, we return a
        negative number.

	Synopsis:
	int nleadkeystr(cchar *bs,cchar *sp,int sl) noex

	Arguments:
	bs		base string to compare against
	sp		test-string to test against the base string
	sl		length of test-string

	Returns:
	>=0		match found and it matched up to this length
	<0		no match (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |INT_MAX| */
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"nleadkeystr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int nleadkeystr(cchar *bs,cchar *sp,int sl) noex {
	int		i{} ;
	if (sl < 0) sl = INT_MAX ;
	for (i = 0 ; (i < sl) && bs[i] && sp[i] ; i += 1) {
	    if (bs[i] != sp[i]) break ;
	    if ((bs[i] == '=') || (sp[i] == '=')) break ;
	} /* end for */
	return i ;
}
/* end subroutine (nleadkeystr) */


