/* getusmtime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getusmtime

	Description:
	This code allows for rertieving the current UNIX® time
	by reading a variable ('getusmtime').

	Synopsis:
	time_t val = getusmtime

	Arguments:
	getusmtime	the variable to read

	Returns:
	>=0		the current UNIX® time (of day)
	<0		error (system-return)


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getusmtime.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

usys_mtime		getusmtime ;


/* exported subroutines */


/* local subroutines */

usys_mtime::operator mtime_t () noex {
	return umtime() ;
} /* end */

mtime_t usys_mtime::operator () () noex {
	return umtime() ;
} /* end */


