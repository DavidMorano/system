/* iswchar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test a wide-character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	iswc{x}

	Aliases:

	Description:
	These subroutines are sort of like |isalpha(3c)| and their
	friends, but allow for ISO Latin-1 (ISO-8859-1) characters
	also.

	Synopsis:
	int iswc{x}(wchar_t wch) noex

	Arguments:
	{x}		white
	wch		wide-character to test

	Returns:
	0		false
	1		true

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bitset>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"iswchar.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cint		cslen		= (UCHAR_MAX + 1) ; /* character-set length */


/* exported variables */


/* exported subroutines */

bool iswcwhite(wchar_t wch) noex {
	bool		f = false ;
	if ((wch >= 0) && (wch < cslen)) {
	    cint ch = conv<int>(wch) ;
	    f = CHAR_ISWHITE(ch) ;
	} /* end if (in lower range) */
	return f ;
} /* end subroutine (iswcwhite) */


