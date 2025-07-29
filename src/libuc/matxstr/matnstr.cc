/* matnstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
	<0		no match found (not further distinguished)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"matnstr.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matnstr(mainv a,cchar *sp,int sl) noex {
	cint		lc = sp[0] ; /* ok: everything promotes the same */
	int		i{} ; /* used-afterwards */
	if (sl < 0) sl = lenstr(sp) ;
	for (i = 0 ; a[i] ; i += 1) {
	    if ((lc == a[i][0]) && (strncmp(a[i],sp,sl) == 0)) break ;
	} /* end for */
	return (a[i] != nullptr) ? i : -1 ;
}
/* end subroutine (matnstr) */


