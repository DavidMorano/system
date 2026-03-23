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
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdclib.hh>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>
#include	<cmporders.h>

#include	"strxxxxcmp.h"


/* local defines */

#define	TOFC(ch)	CHAR_TOFC(ch)

#define	strcasecmp	std_strcasecmp


/* imported namespaces */

using stdclib::std_strcasecmp ;		/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*strcmp_f)(cchar *,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int strxcmp(strcmp_f fun,cchar *s1,cchar *s2,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) {
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
	}
	return rc ;
} /* end subroutine (strxcmp) */


/* local variables */


/* exported variables */


/* exported subroutines */

int strbasecmp(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 && s2) {
	    rc = strcmp(s1,s2) ;
	}
	return rc ;
} /* end subroutine (strbasecmp) */

int strfoldcmp(cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 && s2) {
	    while (rc == 0) {
	        cint ch1 = TOFC(*s1++) ;
	        cint ch2 = TOFC(*s2++) ;
	        rc = ch1 - ch2 ;
	        if (!ch1 || !ch2) break ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strfoldcmp) */


int strbasecmpo(cchar *s1,cchar *s2) noex {
	return (+ strbasecmp(s1,s2)) ;
}
int strbasecmpr(cchar *s1,cchar *s2) noex {
	return (- strbasecmp(s1,s2)) ;
}

int strcasecmpo(cchar *s1,cchar *s2) noex {
	return (+ strcasecmp(s1,s2)) ;
}
int strcasecmpr(cchar *s1,cchar *s2) noex {
	return (- strcasecmp(s1,s2)) ;
}

int strfoldcmpo(cchar *s1,cchar *s2) noex {
	return (+ strfoldcmp(s1,s2)) ;
}
int strfoldcmpr(cchar *s1,cchar *s2) noex {
	return (- strfoldcmp(s1,s2)) ;
}


int strxbasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strbasecmp,s1,s2,cmporder_obverse) ;
}
int strxbasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strbasecmp,s1,s2,cmporder_reverse) ;
}

int strxcasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strcasecmp,s1,s2,cmporder_obverse) ;
}
int strxcasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strcasecmp,s1,s2,cmporder_reverse) ;
}

int strxfoldcmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strfoldcmp,s1,s2,cmporder_obverse) ;
}
int strxfoldcmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strfoldcmp,s1,s2,cmporder_reverse) ;
}


