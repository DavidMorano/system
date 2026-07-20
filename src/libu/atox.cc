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
	ulonglong atoull(cchar *sp) noex

	Arguments:
	sp		c-string to convert

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

slonglong	atosll(cchar *s) noex {
    	slonglong	res = 0 ;
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
		res = strtoxll(s,nullptr,base10) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
    	return res ;
} /* end subroutine (atosll) */

ulonglong	atoull(cchar *s) noex {
    	ulonglong	res = 0 ;
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
		res = strtoxull(s,nullptr,base10) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
    	return res ;
} /* end subroutine (atoull) */


/* local subroutines */


