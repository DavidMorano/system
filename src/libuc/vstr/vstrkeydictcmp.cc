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


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"vstrkeycmpx.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	strkeydictcmp(cchar *,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vstrkeydictcmp(cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	cchar		*s1 = charp(*s1pp) ;
	cchar		*s2 = charp(*s2pp) ;
	if (s1 || s2) {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    rc = strkeydictcmp(s1,s2) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vstrkeydictcmp) */


