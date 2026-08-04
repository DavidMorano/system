/* strbasename SUPPORT deprecated */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the base file name out of a path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strbasename

	Description:
	This routine returns the pointer in the given string of the
	start of the basename portion.

	Note:
	This subroutine is deprecated because it can give an erroneous
	result when the passed (specified) source path c-string ends
	in something like a slash ('/') character.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strbasename(char *sp) noex {
	char		*rp = nullptr ;
	if (sp) ylikely {
	   int		si = 0 ;
	   int		sl = lenstr(sp) ;
	   /* remove trailing slash characters */
	   while ((sl > 1) && (sp[sl - 1] == '/')) {
	       sl -= 1 ;
	   } /* end while */
	   sp[sl] = '\0' ;
	   /* find the next previous slash character */
	   for (si = sl ; si > 0 ; si -= 1) {
	       if (sp[si - 1] == '/') break ;
	   } /* end for */
	   if (sp[1] == '\0') si = 0 ;
	   rp = (sp + si) ;
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strbasename) */


