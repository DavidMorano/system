/* getostype HEADER */
/* lang=C20 */

/* get the type of operating system we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Retrieve the type of operating system.

*******************************************************************************/

#ifndef	GETOSTYPE_INCLUDE
#define	GETOSTYPE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


enum ostypes {
	ostype_bsd,
	ostype_sysv,
	ostype_linux,
	ostype_overlast
} ;


EXTERNC_begin

extern int	getostype() noex ;

EXTERNC_end


#endif /* GETOSTYPE_INCLUDE */


