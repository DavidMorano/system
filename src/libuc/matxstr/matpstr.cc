/* matpstr SUPPORT */
/* lang=C++20 */

/* match a partial string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	matpxstr

	Description:
	Check that the given string matches the LEADING part of
	some string in the given array of strings.  All of the
	strings in the given array of strings are checked and the
	one that matches the most characters (if any) is the one
	whose index is returned.  If we get a match, we return the
	array index.  If we do not match, we return "less-than-zero".

	Synopsis:
	int matpxstr(mainv a,int n,cchar *sp,int sl) noex

	Arguments:
	a		array of string to match against
	n		minimum number of characters that must match
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

	Implementation-note:
	I used to use virtual inheritance to implement subroutine
	groups just like this one in the past. I kind of moved to
	templates more recently (I used to use templates more
	extensively a good white ago for subroutine groups like this).
	But I gave up on templates in the past in preference to
	using virtual inheritance, before moving back to templates
	more recently. But the approach below is a hybrid between
	virtual inheritance and passing subroutine pointers.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<toxc.h>
#include	<nleadstr.h>
#include	<localmisc.h>

#include	"matstr.h"		/* <- needed for one case */
#include	"matpstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int (*toxc_f)(int) noex ;
typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
typedef int (*matxstr_f)(mainv,cchar *,int) noex ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	toxc_f		toxc ;
	nleadxstr_f	nleadx ;
	matxstr_f	matxstr ;
	int matpxstr(mainv,int,cchar *,int) noex ;
	mater(toxc_f t,nleadxstr_f n,matxstr_f m) noex { 
	    toxc = t ;
	    nleadx = n ;
	    matxstr = m ;
	} ; /* end ctor */
    } ; /* end struct (mater) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matpbasestr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tobc,nleadbasestr,matbasestr) ;
	return mo.matpxstr(a,n,sp,sl) ;
}

int matpcasestr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(touc,nleadcasestr,matcasestr) ;
	return mo.matpxstr(a,n,sp,sl) ;
}

int matpfoldstr(mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tofc,nleadfoldstr,matfoldstr) ;
	return mo.matpxstr(a,n,sp,sl) ;
}


/* local subroutines */

int mater::matpxstr(mainv a,int n,cchar *sp,int sl) noex {
	int		si = -1 ;
	if (sl < 0) sl = strlen(sp) ;
	if (n >= 0) {
	    cint	lch = toxc(sp[0]) ;
	    int		m_max = 0 ;
	    int		m ;
	    si = -1 ;
	    for (int i = 0 ; a[i] ; i += 1) {
		if ((m = (lch == toxc(a[i][0]))) > 0) {
		    m = nleadx(a[i],sp,sl) ;
		}
		if ((m == sl) && ((m >= n) || (a[i][m] == '\0'))) {
		    if (m > m_max) {
			m_max = m ;
			si = i ;
		    }
		} /* end if */
	    } /* end for */
	} else {
	    si = matxstr(a,sp,sl) ;
	}
	return si ;
}
/* end subroutine (mater::matpxstr) */


