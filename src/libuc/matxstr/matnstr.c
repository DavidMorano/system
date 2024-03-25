/* matnstr SUPPORT */
/* lang=C20 */

/* match a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matnstr

	Description:
	Check that the given string exactly matches the leading
	part of some string in the given array of strings.  If we
	get a match, we return the array index.  If we do not match,
	we return "less-than-zero".

	Synopsis:
	int matnstr(mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of string to match against
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<string.h>		/* for |strlen(3c)| + |strncmp(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<nleadstr.h>
#include	<localmisc.h>


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matnstr(mainv a,cchar *sp,int sl) noex {
	int		lc = sp[0] ; /* ok: everything promotes the same */
	int		i = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	for (i = 0 ; a[i] ; i += 1) {
	    if ((lc == a[i][0]) && (strncmp(a[i],sp,sl) == 0)) break ;
	} /* end for */
	return (a[i] != nullptr) ? i : -1 ;
}
/* end subroutine (matnstr) */


