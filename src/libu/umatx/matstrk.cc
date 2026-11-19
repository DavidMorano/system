/* matstrk SUPPORT */
/* charset=ISO8859-1 */
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
	matstrk

	Description:
	Check that the key of the given string matches EXACTLY some
	key-string in the given array of strings.  If we get a
	match, we return the array index.  If we do not match, we
	return something "less-than-zero."

	Synopsis:
	int matstrk(con mainv a,cchar *sp,int sl) noex

	Arguments:
	a		array of strings to match against
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ucomx.h>		/* LIBU */
#include	<ucmpx.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"matstrk.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matstrk(con mainv a,cchar *sp,int sl) noex {
	int		i = 0 ; /* return-value */
	bool 		f = false ;
	if (a && sp) ylikely {
	    if (sl > 0) {
	        cint sch = mkchar(sp[0]) ;
	        for ( ; a[i] ; i += 1) {
		    if (sch == mkchar(a[i][0])) {
		        f = cmpstrkeyn(a[i],sp,sl) ;
		    } /* end */
		    if (f) break ;
	        } /* end for */
	    } else if (sl < 0) {
	        cint sch = mkchar(sp[0]) ;
	        for ( ; a[i] ; i += 1) {
	            if (sch == mkchar(a[i][0])) {
		        f = (cmpstrkey(a[i],sp) == 0) ;
		    } /* end */
		    if (f) break ;
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (matstrk) */


