/* atox SUPPORT (ASCII-To-String) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal c-string to an integer type */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-09-03, David A­D­ Morano
	This code was written to provide string-to-integer conversions
	in the style (similar function signatures and semantics)
	of the existing UNIX® standard-C library subroutines like
	|atoi(3c)|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ato{x}

	Names:
	atosll
	atoull

	Synopsis:
	slonglong atosll(cchar *sp) noex
	slonglong atosll(cchar *sp,int b) noex
	ulonglong atoull(cchar *sp) noex
	ulonglong atoull(cchar *sp,int b) noex

	Arguments:
	sp		c-string to convert
	b		number base

	Returns:
	typeof({x})	resulting value in desired integer type
	errno		set on error (UNIX® error number)

	Description:
	This code converts a c-string of decimal digits (where
	base-10 is assumed) into the integer types |slonglong| or
	|ulonglong|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |strtol(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<strtox.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU debugging */

#include	"atox.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		base10	= 10 ;		/* base-10 */
constexpr bool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

slonglong	atosll(cchar *s,int b) noex {
	return strtoxsll(s,nullptr,b) ;
} /* end subroutine (atosll) */

slonglong	atosll(cchar *s) noex {
    	return atosll(s,base10) ;
} /* end subroutine */

ulonglong	atoull(cchar *s,int b) noex {
	return strtoxull(s,nullptr,b) ;
} /* end subroutine (atoull) */

ulonglong	atoull(cchar *s) noex {
    	return atoull(s,base10) ;
} /* end subroutine */


/* local subroutines */


