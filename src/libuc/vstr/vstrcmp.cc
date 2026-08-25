/* vstrcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* v-string comparison (w/ various cases) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	vstr{x}cmp

	Description:
	These subroutines are suitable for the sorting of c-strings
	using other subroutines like |qsort(3c)| or |bsearch(3c)|
	and |vecstr_finder(3uc)|.

	Arguments:
	int vstrbasecmp(cchar **,cchar **) noex
	int vstrcasecmp(cchar **,cchar **) noex
	int vstrfoldcmp(cchar **,cchar **) noex

	Returns:
	-		three-way comparison value

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdclib.hh>		/* LIBU |std_strcasecmp(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<strxcmp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<vstrorders.h>		/* LIBU */

#include	"vstrcmp.h"


/* local defines */

#define	strcasecmp	std_strcasecmp


/* imported namespaces */

using stdclib::std_strcasecmp ;		/* subroutine */


/* local typedefs */

typedef vstrorders	vo ;

extern "C" {
    typedef int (*strxcmp_f)(cc *,cc *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int vstrxcmp(strxcmp_f vx,cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	if (s1pp && s2pp) ylikely {
	    cchar	*s1 = *s1pp ;
	    cchar	*s2 = *s2pp ;
	    if (s1 || s2) ylikely {
	        rc = +1 ;
	        if (s1) {
		    rc = -1 ;
		    if (s2) {
		        rc = vx(s1,s2) ;
		    }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (vstrcmpx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vstrbasecmp(cchar **e1pp,cchar **e2pp) noex { /* base */
	return vstrxcmp(strbasecmp,e1pp,e2pp) ;
} /* end subroutine (vstrbasecmp) */

int vstrcasecmp(cchar **e1pp,cchar **e2pp) noex { /* case */
	return vstrxcmp(strcasecmp,e1pp,e2pp) ;
} /* end subroutine (vstrcasecmp) */

int vstrfoldcmp(cchar **e1pp,cchar **e2pp) noex { /* fold */
	return vstrxcmp(strfoldcmp,e1pp,e2pp) ;
} /* end subroutine (vstrfoldcmp) */


int vstrbasecmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* base */
	strxcmp_f cmpfun = (fo) ? strbasecmpr : strbasecmpo ;
	return vstrxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrbasecmpx) */

int vstrcasecmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* case */
	strxcmp_f cmpfun = (fo) ? strcasecmpr : strcasecmpo ;
	return vstrxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrcasecmpx) */

int vstrfoldcmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* fold */
	strxcmp_f cmpfun = (fo) ? strfoldcmpr : strfoldcmpo ;
	return vstrxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrfoldcmpx) */


