/* vstrkeycmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* v-string key comparison (obverse and reverse) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Hroup:
	vstrkey{x}cmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int vstrkeybasecmp(cchar **s1pp,cchar **s2pp) noex
	int vstrkeycasecmp(cchar **s1pp,cchar **s2pp) noex
	int vstrkeyfoldcmp(cchar **s1pp,cchar **s2pp) noex

	Arguments:
	s1pp		c-string pointer to pointer
	s2pp		c-string pointer to pointer

	Returns:
	<0		less than
	==0		equal
	>0		greater than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strkeycmp.h>
#include	<localmisc.h>
#include	<vstrorders.h>

#include	"vstrkeycmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef vstrorders	vo ;

extern "C" {
    typedef int (*strxcmp_f)(cchar *,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int vstrkeyxcmp(strxcmp_f vx,cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	if (s1pp && s2pp) {
	    cchar	*s1 = *s1pp ;
	    cchar	*s2 = *s2pp ;
	    if (s1 || s2) {
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
} /* end subroutine (vstrkeyxcmp) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vstrkeybasecmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeyxcmp(strkeybasecmp,s1pp,s2pp) ;
}

int vstrkeycasecmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeyxcmp(strkeycasecmp,s1pp,s2pp) ;
}

int vstrkeyfoldcmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeyxcmp(strkeyfoldcmp,s1pp,s2pp) ;
}


int vstrkeybasecmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* base */
	strxcmp_f cmpfun = (fo) ? strkeybasecmpr : strkeybasecmpo ;
	return vstrkeyxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrkeybasecmpx) */

int vstrkeycasecmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* case */
	strxcmp_f cmpfun = (fo) ? strkeycasecmpr : strkeycasecmpo ;
	return vstrkeyxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrkeycasecmpx) */

int vstrkeyfoldcmpx(cchar **e1pp,cchar **e2pp,vo fo) noex { /* fold */
	strxcmp_f cmpfun = (fo) ? strkeyfoldcmpr : strkeyfoldcmpo ;
	return vstrkeyxcmp(cmpfun,e1pp,e2pp) ;
} /* end subroutine (vstrkeyfoldcmpx) */


