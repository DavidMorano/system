/* matstrm SUPPORT (Match Prefix String) */
/* charset=ISO8859-1 */
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
	matp{x}str

	Description:
	Check that the given string matches the LEADING part of
	some string in the given array of strings.  All of the
	strings in the given array of strings are checked and the
	one that matches the most characters (if any) is the one
	whose index is returned.  If we get a match, we return the
	array index.  If we do not match, we return "less-than-zero".

	Synopsis:
	int matp{x}str(con mainv a,int n,cchar *sp,int sl) noex

	Arguments:
	{x}		base, case, fold
	a		array of string to match against
	n		minimum number of characters that must match
	sp		string to test against array
	sl		length of test string

	Returns:
	>=0		index of match in array
	<0		no match found (not further distinguished)

	Implementation-note:
	I used to use virtual inheritance to implement subroutine
	groups just like this one in the past.  I kind of moved to
	templates more recently (I used to use templates more
	extensively a good white ago for subroutine groups like this).
	But I gave up on templates in the past in preference to
	using virtual inheritance, before moving back to templates
	more recently.  But the approach below is a hybrid between
	virtual inheritance and passing subroutine pointers.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |toxc(3u)| */
#include	<comstr.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"matstr.h"		/* <- needed for one case */
#include	"matstrm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)	(int) noex ;
    typedef int (*comx_f)	(cchar *,cchar *,int) noex ;
    typedef int (*matx_f)	(con mainv,cchar *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mater {
	toxc_f		toxc ;
	comx_f		comx ;
	matx_f		matx ;
	int operator () (con mainv,int,cchar *,int) noex ;
	mater(toxc_f t,comx_f n,matx_f m) noex { 
	    toxc = t ;
	    comx = n ;
	    matx = m ;
	} ; /* end ctor */
    } ; /* end struct (mater) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int matpbasestr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tobc,comstrbc,matstrbc) ;
	return mo(a,n,sp,sl) ;
} /* end subroutine */

int matpcasestr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(touc,comstric,matstric) ;
	return mo(a,n,sp,sl) ;
} /* end subroutine */

int matpfoldstr(con mainv a,int n,cchar *sp,int sl) noex {
	mater	mo(tofc,comstrfc,matstrfc) ;
	return mo(a,n,sp,sl) ;
} /* end subroutine */


/* local subroutines */

int mater::operator () (con mainv a,int n,cchar *sp,int µsl) noex {
	int		si = -1 ;
	if (int sl ; a && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    if (n >= 0) {
	        cint	lch = toxc(sp[0]) ;
	        int	m_max = 0 ;
	        for (int m, i = 0 ; a[i] ; i += 1) {
		    if ((m = (lch == toxc(a[i][0]))) > 0) {
		        m = comx(a[i],sp,sl) ;
		    } /* end */
		    if ((m == sl) && ((m >= n) || (a[i][m] == '\0'))) {
		        if (m > m_max) {
			    m_max = m ;
			    si = i ;
		        } /* end */
		    } /* end if */
	        } /* end for */
	    } else {
	        si = matx(a,sp,sl) ;
	    } /* end */
	} /* end if (getlenstr) */
	return si ;
} /* end subroutine (mater::operator) */


