/* strwcmp SUPPORT */
/* lang=C++20 */

/* compare the minimum common characters of two strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for a convenience.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwcmp

	Description: 
	This subroutine compares two strings, but the second string
	is allowed to be counted instead of NUL terminated.

	Synopsis:
	int strwcmp(cchar *s1,cchar *s2,int s2len) noex

	Arguments:
	s1	one string
	s2	second string
	s2len	length of second string

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<nleadstr.h>

#include	"strwcmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local variables */


/* external subroutines */


/* exported subroutines */

int strwcmpx(cchar *s1,cchar *s2,int s2len) noex {
	int		rc = mkchar(s1[0]) ;
	if (s2len < 0) s2len = strlen(s2) ;
	if (s2len > 0) {
	    cint	ch1 = mkchar(*s1) ;
	    cint	ch2 = mkchar(*s2) ;
	    if ((rc = (ch1 - ch2)) == 0) {
	        if ((rc = strncmp(s1,s2,s2len)) == 0) {
	            cint	m = nleadstr(s1,s2,s2len) ;
	            if (m < s2len) {
	    	        cint	m1 = mkchar(s1[m]) ;
	    	        cint	m2 = mkchar(s2[m]) ;
		        rc = (m1 - m2) ;
		    } else {
		        rc = mkchar(s1[m]) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end subroutine (strwcmpx) */

int strwcasecmpx(cchar *s1,cchar *s2,int s2len) noex {
	int		rc = tolc(s1[0]) ;
	if (s2len < 0) s2len = strlen(s2) ;
	if (s2len > 0) {
	    cint	ch1 = tolc(*s1) ;
	    cint	ch2 = tolc(*s2) ;
	    if ((rc = (ch1 - ch2)) == 0) {
	        if ((rc = strncasecmp(s1,s2,s2len)) == 0) {
	            cint	m = nleadcasestr(s1,s2,s2len) ;
	            if (m < s2len) {
	    	        cint	m1 = tolc(s1[m]) ;
	    	        cint	m2 = tolc(s2[m]) ;
		        rc = (m1 - m2) ;
		    } else {
		        rc = tolc(s1[m]) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end subroutine (strwcasecmpx) */


