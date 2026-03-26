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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasnon.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */


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
	    while (sl && *sp) {
	        f = (! CHAR_ISWHITE(*sp)) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnonwhite) */


/* local subroutines */


