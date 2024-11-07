/* getostype HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get the type of operating system we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A.D. Morano
	This was written from scratch.

*/

/* Copyright � 2003 David A�D� Morano.  All rights reserved. */

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
	>=0		0=BSD, 1=SYSV
	<0		error (system-return)

*******************************************************************************/

#ifndef	GETOSTYPE_INCLUDE
#define	GETOSTYPE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum ostypes {
	ostype_bsd,
	ostype_sysv,
	ostype_linux,
	ostype_darwin,
	ostype_overlast
} ;

EXTERNC_begin
extern int	getostype() noex ;
EXTERNC_end

#ifdef	__cplusplus

namespace libdam {
    struct ostyper {
        operator int () noex ;
        int operator () () noex ;
    } ; /* end struct (ostyper) */
}

extern libdam::ostyper	ostype ;

extern cpcchar		ostypenames[] ;

#endif /* __cplusplus */


#endif /* GETOSTYPE_INCLUDE */


