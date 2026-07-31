/* hasnon SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasnon{x}

	Description:
	These subroutines check if a specified counted c-string
	does *not* have any of some characteristic we are looking
	for.


	Name:
	hasnonwhite

	Description:
	Determine if the given c-string has a non-white character.

	Synopsis:
	bool hasnonwhite(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string is empty
	true		c-string has some non-white content

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"hasnon.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasnonwhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    for ( ; sl-- && *sp ; sp += 1) {
	        if ((f = (! ISWHT(*sp)))) break ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasnonwhite) */


/* local subroutines */


