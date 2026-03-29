/* getostype HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the type of operating system we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getostype

	Description:
	This subroutine returns the type of operating system it is
	currently running on.

	Synopsis:
	int getostype() noex

	Arguments:
	-

	Returns:
	>=0		OS-type:
				ostype_bsd,
				ostype_sysv,
				ostype_linux,
				ostype_darwin,
	<0		error (system-return)

*******************************************************************************/

#ifndef	GETOSTYPE_INCLUDE
#define	GETOSTYPE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum ostypes {
	ostype_bsd,
	ostype_sysv,
	ostype_linux,
	ostype_darwin,
	ostype_overlast
} ; /* end enum (ostypes) */

EXTERNC_begin

extern int	getostype() noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libdam {
    struct ostyper {
        operator int () noex ;
        int operator () () noex ;
    } ; /* end struct (ostyper) */
} /* end namespace (libdam) */

extern libdam::ostyper	ostype ;

extern cpcchar		ostypenames[] ;

#endif /* __cplusplus */


#endif /* GETOSTYPE_INCLUDE */


