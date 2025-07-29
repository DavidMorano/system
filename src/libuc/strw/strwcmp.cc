/* strwcmp SUPPORT */
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

	Name:
	strwcmp
	strw{x}cmp

	Description: 
	This subroutine compares two strings, but the second string
	is allowed to be counted instead of NUL terminated.

	Synopsis:
	int strwcmp(cchar *s1,cchar *s2,int s2len) noex
	int strw{x}cmp(cchar *s1,cchar *s2,int s2len) noex

	Arguments:
	{×}	'base' 'case' 'fold'
	s1	one string
	s2	second string
	s2len	length of second string

	Returns:
	>0	the first string is greater than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>
#include	<strnxcmp.h>		/* |strnfoldcmp(3uc)| */
#include	<toxc.h>

#include	"strwcmp.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
   struct cmpx {
	int x(cchar *,cchar *,int) noex ;
	virtual int tox(int ch) noex = 0 ;
	virtual int strnxcmp(cchar *,cchar *,int) noex = 0 ;
	virtual int nleadxstr(cchar *,cchar *,int) noex = 0 ;
   } ; /* end struct (cmpx) */
   struct basecmpx : cmpx {
	int tox(int ch)					noex override final {
	    return ch ;
	} ;
	int strnxcmp(cchar *bs,cchar *sp,int sl)	noex override final {
	    return strncmp(bs,sp,sl) ;
	} ;
	int nleadxstr(cchar *s1,cchar *s2,int s2len)	noex override final {
	    return nleadstr(s1,s2,s2len) ;
	} ;
   } ;
   struct casecmpx : cmpx {
	int tox(int ch)					noex override final {
	    return tolc(ch) ;
	} ;
	int strnxcmp(cchar *bs,cchar *sp,int sl)	noex override final {
	    return strncasecmp(bs,sp,sl) ;
	} ;
	int nleadxstr(cchar *s1,cchar *s2,int s2len)	noex override final {
	    return nleadcasestr(s1,s2,s2len) ;
	} ;
   } ;
   struct foldcmpx : cmpx {
	int tox(int ch)					noex override final {
	    return tofc(ch) ;
	} ;
	int strnxcmp(cchar *bs,cchar *sp,int sl)	noex override final {
	    return strnfoldcmp(bs,sp,sl) ;
	} ;
	int nleadxstr(cchar *s1,cchar *s2,int s2len)	noex override final {
	    return nleadfoldstr(s1,s2,s2len) ;
	} ;
   } ;
} /* end namespace */


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

int strwbasecmp(cchar *s1,cchar *s2,int s2len) noex {
	basecmpx	co ;
	return co.x(s1,s2,s2len) ;
}
/* end subroutine (strwbasecmpx) */

int strwcasecmp(cchar *s1,cchar *s2,int s2len) noex {
	casecmpx	co ;
	return co.x(s1,s2,s2len) ;
}
/* end subroutine (strwcasecmpx) */

int strwfoldcmp(cchar *s1,cchar *s2,int s2len) noex {
	foldcmpx	co ;
	return co.x(s1,s2,s2len) ;
}
/* end subroutine (strwfoldcmpx) */


/* local subroutines */

int cmpx::x(cchar *s1,cchar *s2,int s2len) noex {
	int		rc = tox(s1[0]) ;
	if (s2len < 0) {
	    s2len = lenstr(s2) ;
	}
	if (s2len > 0) {
	    cint	ch1 = tox(*s1) ;
	    cint	ch2 = tox(*s2) ;
	    if ((rc = (ch1 - ch2)) == 0) {
	        if ((rc = strnxcmp(s1,s2,s2len)) == 0) {
	            if (cint m = nleadxstr(s1,s2,s2len) ; m < s2len) {
	    	        cint	m1 = tox(s1[m]) ;
	    	        cint	m2 = tox(s2[m]) ;
		        rc = (m1 - m2) ;
		    } else {
		        rc = tox(s1[m]) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end method (cmpx::x) */


