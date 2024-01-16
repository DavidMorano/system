/* vstrkeycmpx SUPPORT */
/* lang=C20 */

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdlib.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<strkeycmp.h>

#include	"vstrkeycmpx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int vstrkeycmpx(cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	cchar		*s1 = (cchar *) *s1pp ;
	cchar		*s2 = (cchar *) *s2pp ;
	if (s1 || s2) {
	    rc = 1 ;
	    if (s1) {
		rc = -1 ;
	        if (s2) {
		    cint	ch1 = mkchar(*s1) ;
		    cint	ch2 = mkchar(*s2) ;
	            if ((rc = (ch1 - ch2)) == 0) {
	                rc = strkeycmp(s1,s2) ;
	            }
		}
	    }
	} /* end if */
	return rc ;
}
/* end subroutine (vstrkeycmpx) */


/* local variables */


/* exported subroutines */

int vstrkeycmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeycmpx(s1pp,s2pp) ;
}

int vstrkeycmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeycmpx(s2pp,s1pp) ;
}


