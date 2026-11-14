/* cmpvstr SUPPORT (Compare-V-String) */
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
	int cmpvstrbc(cchar **,cchar **) noex
	int cmpvstric(cchar **,cchar **) noex
	int cmpvstrfc(cchar **,cchar **) noex

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
#include	<mkchar.h>		/* LIBU */
#include	<cmpstr.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"cmpvstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef cmporders	vo ;

extern "C" {
    typedef int (*cmpstr_f)(cc *,cc *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int cmpvstrx(cmpstr_f vx,cchar **s1pp,cchar **s2pp) noex {
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
} /* end subroutine (cmpvstrx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpvstrbc(cchar **e1pp,cchar **e2pp) noex { /* base */
	return cmpvstrx(cmpstrbc,e1pp,e2pp) ;
} /* end subroutine (cmpvstrbc) */

int cmpvstric(cchar **e1pp,cchar **e2pp) noex { /* case */
	return cmpvstrx(cmpstric,e1pp,e2pp) ;
} /* end subroutine (cmpvstric) */

int cmpvstrfc(cchar **e1pp,cchar **e2pp) noex { /* fold */
	return cmpvstrx(cmpstrfc,e1pp,e2pp) ;
} /* end subroutine (cmpvstrfc) */


int cmpvstrbcx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* base */
	cmpstr_f cmpfun = (fo) ? cmpstrbcr : cmpstrbco ;
	return cmpvstrx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstrbcx) */

int cmpvstricx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* case */
	cmpstr_f cmpfun = (fo) ? cmpstricr : cmpstrico ;
	return cmpvstrx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstricx) */

int cmpvstrfcx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* fold */
	cmpstr_f cmpfun = (fo) ? cmpstrfcr : cmpstrfco ;
	return cmpvstrx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstrfcx) */


