/* strxxxxcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string comparison w/ folded case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strfoldcmp

	Description:
	This subroutine performs a c-string comparisonn using folded
	case.  This is almost identical to |strncasecmp(3c)| except
	that the comparison is done using folded case.

	Synopsis:
	int strfoldcmp(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		first string
	e2p		second string

	Returns:
	>0		first c-string is greater than the second
	==0		both c-strings are equal
	<0		first c-string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdclib.hh>		/* LIBU */
#include	<toxc.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strxxxxcmp.h"


/* local defines */


/* imported namespaces */

using stdclib::std_strcasecmp ;		/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*strcmp_f)(cchar *,cchar *) noex ;
    typedef int (*toxc_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int strxxxxcmp(toxc_f tox,cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    while (rc == 0) {
	        cint ch1 = tox(*s1++) ;
	        cint ch2 = tox(*s2++) ;
	        rc = ch1 - ch2 ;
	        if (!ch1 || !ch2) break ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strxxxxcmp) */

local int strxcmp(strcmp_f fun,cchar *s1,cchar *s2,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) ylikely {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    if (fo) {
		        rc = (- fun(s1,s2)) ;
		    } else {
		        rc = (+ fun(s1,s2)) ;
		    }
		}
	    }
	} /* end if */
	return rc ;
} /* end subroutine (strxcmp) */


/* local variables */


/* exported variables */


/* exported subroutines */

int strbasecmp(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = strcmp(s1,s2) ;
	}
	return rc ;
} /* end subroutine (strbasecmp) */

int strcasecmp_latin(cchar *s1,cchar *s2) noex {
    	return strxxxxcmp(touc,s1,s2) ;
} /* end subroutine (strfoldcmp_latin) */

int strfoldcmp(cchar *s1,cchar *s2) noex {
    	return strxxxxcmp(tofc,s1,s2) ;
} /* end subroutine (strfoldcmp) */

int strbasecmpo(cchar *s1,cchar *s2) noex {
	return (+ strbasecmp(s1,s2)) ;
} /* end subroutine */

int strbasecmpr(cchar *s1,cchar *s2) noex {
	return (- strbasecmp(s1,s2)) ;
} /* end subroutine */

int strcasecmpo(cchar *s1,cchar *s2) noex {
	return (+ strcasecmp_latin(s1,s2)) ;
} /* end subroutine */

int strcasecmpr(cchar *s1,cchar *s2) noex {
	return (- strcasecmp_latin(s1,s2)) ;
} /* end subroutine */

int strfoldcmpo(cchar *s1,cchar *s2) noex {
	return (+ strfoldcmp(s1,s2)) ;
} /* end subroutine */

int strfoldcmpr(cchar *s1,cchar *s2) noex {
	return (- strfoldcmp(s1,s2)) ;
} /* end subroutine */

int strxbasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strbasecmp,s1,s2,cmporder_obverse) ;
} /* end subroutine */

int strxbasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strbasecmp,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int strxcasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strcasecmp_latin,s1,s2,cmporder_obverse) ;
} /* end subroutine */

int strxcasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strcasecmp_latin,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int strxfoldcmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strfoldcmp,s1,s2,cmporder_obverse) ;
} /* end subroutine */

int strxfoldcmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strfoldcmp,s1,s2,cmporder_reverse) ;
} /* end subroutine */


