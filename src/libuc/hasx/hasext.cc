/* hasext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* does the c-string have the given extension c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasext

	Description:
	Determine if the given c-string contains a specified
	c-string extension.

	Synopsis:
	bool hasext(cchar *sp,int sl,cchar *ext) noex

	Arguments:
	sp		c-string pointer
	sl		c-string length
	ext		extension c-string to search for

	Returns:
	true		the given c-string has a leading given character
	false		the given c-string did not match


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<six.h>			/* LIBUC |sispanwht(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"hasext.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int siext(cchar *,int) noex ;
} /* end */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasext(cchar *sp,int sl,cchar *ext) noex {
    	bool		f = false ;
	if (int si = siext(sp,sl) ; si >= 0) {
	    f = (strcmp((sp + si),ext) == 0) ;
	} /* end if (siext) */
    	return f ;
} /* end subroutine (hasext) */


