/* substring SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* routine to match a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	substring

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not in the buffer specified by the first two
	arguments.  This subroutine either returns (-1) or it returns
	the character position in the buffer of where the string
	starts.

	Synopsis:
	int substring(cchar *s1,int len,cchar *s2) noex

	Arguments:
	s1		string to be examined
	len		length of string to be examined
	s2		null terminated substring to search for

	Returns:
	>=0		substring was found in the main string at index
	<0		substring was not found in main string

******************************************************************************/ 

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<six.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"substring.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int substring(cchar *sp,int sl,cchar *ss) noex {
	return sisub(sp,sl,ss) ;
} /* end subroutine (substring) */


