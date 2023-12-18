/* strcpyxc */
/* lang=C++20 */

/* copy a c-string to given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine converts a string to *changed* case.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"strcpyxc.h"


/* local defines */


/* local typedefs */

typedef int (*toxc_f)(int) noex ;


/* external subroutines */


/* local varialbes */


/* local subroutines */

char *strcpyxc(toxc_f toxc,char *dp,cchar *sp) noex {
	while (*sp) {
	    *dp++ = toxc(*sp++) ;
	} /* end while */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strcpyxc) */


/* exported subroutines */

char *strcpybc(char *dp,cchar *sp) noex {
	return strcpyxc(tobc,dp,sp) ;
}

char *strcpylc(char *dp,cchar *sp) noex {
	return strcpyxc(tolc,dp,sp) ;
}

char *strcpyuc(char *dp,cchar *sp) noex {
	return strcpyxc(touc,dp,sp) ;
}

char *strcpyfc(char *dp,cchar *sp) noex {
	return strcpyxc(tofc,dp,sp) ;
}


