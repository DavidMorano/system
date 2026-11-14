/* cmpvstrk SUPPORT (Compare-V-String-Key) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT v-string key comparison (obverse and reverse) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	vstrkey{x}cmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int cmpvstrkbc(cchar **s1pp,cchar **s2pp) noex
	int cmpvstrkic(cchar **s1pp,cchar **s2pp) noex
	int cmpvstrkfc(cchar **s1pp,cchar **s2pp) noex

	Arguments:
	s1pp		c-string pointer to pointer
	s2pp		c-string pointer to pointer

	Returns:
	<0		less than
	==0		equal
	>0		greater than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"cmpstrk.h"
#include	"cmpvstrk.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef cmporders	vo ;

extern "C" {
    typedef int (*cmpstrkx_f)(cchar *,cchar *) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int cmpvstrkx(cmpstrkx_f fun,cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	if (s1pp && s2pp) {
	    cchar	*s1 = *s1pp ;
	    cchar	*s2 = *s2pp ;
	    if (s1 || s2) {
	        rc = +1 ;
	        if (s1) {
		    rc = -1 ;
	            if (s2) {
		        rc = fun(s1,s2) ;
		    }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (cmpvstrkx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpvstrkbc(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkx(cmpstrkbc,s1pp,s2pp) ;
} /* end */
int cmpvstrkic(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkx(cmpstrkic,s1pp,s2pp) ;
} /* end */
int cmpvstrkfc(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkx(cmpstrkfc,s1pp,s2pp) ;
} /* end */

int cmpvstrkbcx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* base */
	cmpstrkx_f cmpfun = (fo) ? cmpstrkbcr : cmpstrkbco ;
	return cmpvstrkx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstrkbcx) */
int cmpvstrkicx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* case */
	cmpstrkx_f cmpfun = (fo) ? cmpstrkicr : cmpstrkico ;
	return cmpvstrkx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstrkicx) */
int cmpvstrkfcx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* fold */
	cmpstrkx_f cmpfun = (fo) ? cmpstrkfcr : cmpstrkfco ;
	return cmpvstrkx(cmpfun,e1pp,e2pp) ;
} /* end subroutine (cmpvstrkfcx) */


