/* vstrkeycmpx SUPPORT */
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

	Name:
	vstrkeycmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int vstrkeycmpo(cchar **s1pp,cchar **s2pp) noex
	int vstrkeycmpr(cchar **s1pp,cchar **s2pp) noex

	Arguments:

	Returns:
	<0		less than
	==0		equal
	>0		greater

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<strkeycmp.h>

#include	"vstrkeycmpx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int (*verse_f)(cc *,cc *) noex ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int vstrkeycmpx(verse_f vx,cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	cchar		*s1 = charp(*s1pp) ;
	cchar		*s2 = charp(*s2pp) ;
	if (s1 || s2) {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
	        if (s2) {
		    rc = vx(s1,s2) ;
		}
	    }
	} /* end if */
	return rc ;
}
/* end subroutine (vstrkeycmpx) */

static int obverse(cc *s1,cc *s2) noex {
	cint	ch1 = mkchar(*s1) ;
	cint	ch2 = mkchar(*s2) ;
	int	rc ;
	if ((rc = (ch1 - ch2)) == 0) {
	    rc = strkeycmp(s1,s2) ;
	}
	return rc ;
}

static int reverse(cc *s1,cc *s2) noex {
	cint	ch1 = mkchar(*s1) ;
	cint	ch2 = mkchar(*s2) ;
	int	rc ;
	if ((rc = (ch2 - ch1)) == 0) {
	    rc = strkeycmp(s2,s1) ;
	}
	return rc ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int vstrkeycmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeycmpx(obverse,s1pp,s2pp) ;
}

int vstrkeycmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeycmpx(reverse,s2pp,s1pp) ;
}


