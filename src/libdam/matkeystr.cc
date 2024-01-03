/* matkeystr */
/* lang=C++20 */

/* match the key part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matkeystr

	Description:
	Check that the key of the given string matches EXACTLY some
	key-string in the given array of strings.  If we get a
	match, we return the array index.  If we do not match, we
	return something "less-than-zero."

	Synopsis:
	int matkeystr(cchar **a,cchar *sp,int sl) noex

	Arguments:
	a		array of strings to match against
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<nleadkeystr.h>
#include	<strkeycmp.h>
#include	<localmisc.h>

#include	"matkeystr.h"


/* local defines */


/* external subroutines */


/* local variables */


/* local subroutines */

static inline bool keyend(cint ch) noex {
	return (((ch) == '\0') || ((ch) == '=')) ;
}


/* exported subroutines */

int matkeystr(mainv a,cchar *sp,int sl) noex {
	cint		sch = sp[0] ; /* ok: everything promotes the same */
	int		i ;
	int		f = FALSE ;
	if (sl >= 0) {
	    int		m ;
	    for (i = 0 ; a[i] != NULL ; i += 1) {
		m = (sch == a[i][0]) ;
		if (m > 0) {
		    m = nleadkeystr(a[i],sp,sl) ;
		}
		f = (((m == sl) || keyend(sp[m])) && keyend(a[i][m])) ;
		if (f) break ;
	    } /* end for */
	} else {
	    for (i = 0 ; a[i] != NULL ; i += 1) {
	        if (sch == a[i][0]) {
		    f = (strkeycmp(a[i],sp) == 0) ;
		} else {
		    f = (keyend(sp[0]) && keyend(a[i][0])) ;
		}
		if (f) break ;
	    } /* end for */
	} /* end if */
	return (f) ? i : -1 ;
}
/* end subroutine (matkeystr) */


