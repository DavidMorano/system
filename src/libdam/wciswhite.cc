/* wciswhite SUPPORT */
/* lang=C++20 */

/* check for a white-space wide-character */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We check for a white-space wide-character.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>
#include	<char.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    bool	wcishite(wchar_t) noex ;
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool wciswhite(wchar_t ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 256)) {
	    f = CHAR_ISWHITE(ch) ;
	}
	return f ;
}
/* end subroutine (wciswhite) */


