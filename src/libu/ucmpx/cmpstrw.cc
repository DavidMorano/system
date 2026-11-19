/* cmpstrw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* compare a (NUL-terminated) base c-string with a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for a convenience.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	cmpstrw

	Description: 
	This subroutine compares two strings, but the second string
	is allowed to be counted instead of NUL terminated.

	Synopsis:
	int cmpstrw{x}c(cchar *s1,cchar *s2,int s2len) noex

	Arguments:
	{×}	'base' 'case' 'fold'
	s1	c-string pointer first
	s2	c-string pointer second
	s2len	length of second string

	Returns:
	>0	first string is greater than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<comstr.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cmpstrn.h"
#include	"cmpstrw.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
   struct cmper {
	int x(cc *,cc *,int)			const noex ;
	virtual int tox(int ch) 		const noex = 0 ;
	virtual int cmpx(cc *,cc *,int) 	const noex = 0 ;
	virtual int comx(cc *,cc *,int)		const noex = 0 ;
   } ; /* end struct (cmpx) */
   struct cmpxb : cmper {
	int tox(int ch)				const noex override final {
	    return (ch & UCHAR_MAX) ;
	} ; /* end */
	int cmpx(cc *bs,cc *sp,int sl)		const noex override final {
	    return cmpstrnbc(bs,sp,sl) ;
	} ; /* end */
	int comx(cc *s1,cc *s2,int sl)		const noex override final {
	    return comstrbc(s1,s2,sl) ;
	} ; /* end */
   } ; /* end struct */
   struct cmpxi : cmper {
	int tox(int ch)				const noex override final {
	    return touc(ch & UCHAR_MAX) ;
	} ; /* end */
	int cmpx(cc *bs,cc *sp,int sl)		const noex override final {
	    return cmpstrnic(bs,sp,sl) ;
	} ; /* end */
	int comx(cc *s1,cc *s2,int sl)		const noex override final {
	    return comstric(s1,s2,sl) ;
	} ; /* end */
   } ; /* end struct */
   struct cmpxf : cmper {
	int tox(int ch)				const noex override final {
	    return tofc(ch & UCHAR_MAX) ;
	} ; /* end */
	int cmpx(cc *bs,cc *sp,int sl)		const noex override final {
	    return cmpstrnfc(bs,sp,sl) ;
	} ; /* end */
	int comx(cc *s1,cc *s2,int sl)		const noex override final {
	    return comstrfc(s1,s2,sl) ;
	} ; /* end */
   } ; /* end struct */
} /* end namespace */


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

int cmpstrwbc(cchar *s1,cchar *s2,int s2len) noex {
	cmpxb co ;
	return co.x(s1,s2,s2len) ;
} /* end subroutine (strwcmpxb) */

int cmpstrwic(cchar *s1,cchar *s2,int s2len) noex {
	cmpxi co ;
	return co.x(s1,s2,s2len) ;
} /* end subroutine (strwcmpxi) */

int cmpstrwfc(cchar *s1,cchar *s2,int s2len) noex {
	cmpxf co ;
	return co.x(s1,s2,s2len) ;
} /* end subroutine (strwcmpxf) */


/* local subroutines */

int cmper::x(cchar *s1,cchar *s2,int µs2len) const noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = tox(s1[0]) ;
	    if (cint s2len = getlenstr(s2,µs2len) ; s2len > 0) ylikely {
	        cint	ch1 = tox(*s1) ;
	        cint	ch2 = tox(*s2) ;
	        if ((rc = (ch1 - ch2)) == 0) {
	            if ((rc = cmpx(s1,s2,s2len)) == 0) {
	                if (cint m = comx(s1,s2,s2len) ; m < s2len) {
	    	            cint m1 = tox(s1[m]) ;
	    	            cint m2 = tox(s2[m]) ;
		            rc = (m1 - m2) ;
		        } else {
		            rc = tox(s1[m]) ;
		        } /* end */
		    } /* end if (cmpx) */
	        } /* end if (first characters were equal) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rc ;
} /* end method (cmper::x) */


