/* vstrkeydictcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vstrkeydictcmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string :
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int vstrkeydictcmp(cchar **,cchar **) noex

	Returns:
	-		value

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>
#include	<vstrorder.h>

#include	"vstrkeydictcmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef vstrorders	vo ;


/* external subroutines */

extern "C" {
    extern int	strkeydictcmp(cchar *,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

local int vstrkeydictcmpx(cchar **s1pp,cchar **s2pp,vo fo) noex {
	int		rc = 0 ;
	if (s1pp && s2pp) {
	    cchar	*s1 = *s1pp ;
	    cchar	*s2 = *s2pp ;
	    if (s1 || s2) {
	        rc = +1 ;
	        if (s1) {
		    rc = -1 ;
		    if (s2) {
			if (fo) {
		            rc = strkeydictcmp(s2,s1) ;
			} else {
		            rc = strkeydictcmp(s1,s2) ;
			}
		    }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (vstrkeydictcmpx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vstrkeydictcmpo(cchar **s1pp,cchar **s2pp) noex {
    	return vstrkeydictcmpx(s1pp,s2pp,vstrorder_obverse) ;
}

int vstrkeydictcmpr(cchar **s1pp,cchar **s2pp) noex {
    	return vstrkeydictcmpx(s1pp,s2pp,vstrorder_reverse) ;
}


