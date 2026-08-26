/* wciswhite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check for a white-space wide-character */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	wciswhite

	Description:
	I check for a white-space wide-character.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"wcuswhite.h"


/* local defines */


/* external subroutines */

extern "C" {
    bool	wcishite(wchar_t) noex ;
} /* end extern (C) */


/* forward references */


/* local variables */

cint		cslen		= (UCHAR_MAX + 1) ;


/* exported variables */


/* exported subroutines */

bool wciswhite(wchar_t ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < cslen)) {
	    f = CHAR_ISWHITE(ch) ;
	}
	return f ;
} /* end subroutine (wciswhite) */


